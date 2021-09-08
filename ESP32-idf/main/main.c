#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"
#include "esp_netif.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include "esp_tls.h"
#include "esp_crt_bundle.h"
/* Constants that aren't configurable in menuconfig */
#define WEB_SERVER "192.168.0.12"
#define WEB_PORT "9999"
#define WEB_URL "https://192.168.0.12:9999/authenticate"
// #define WEB_URL "https://192.168.0.12:9999/"

static const char REQUEST[] = "GET " WEB_URL " HTTP/1.1\r\n"
                             "Host: "WEB_SERVER"\r\n"
                             "User-Agent: esp-idf/1.0 esp32\r\n"
                             "\r\n";

#define MAX_URLS    5

static const char *web_urls[MAX_URLS] = {
    "https://192.168.0.12:9999",
    "https://aulas-sustentables.herokuapp.com",
    "https://github.com",
    "https://firebase.google.com",
    "https://finalproject-35a1b.firebaseio.com"
};

static const char *INFO_TAG = "info";

extern const uint8_t node_cert_pem_start[] asm("_binary_node_cert_pem_start");
extern const uint8_t node_cert_pem_end[]   asm("_binary_node_cert_pem_end");

extern const uint8_t node_key_pem_start[] asm("_binary_node_key_pem_start");
extern const uint8_t node_key_pem_end[]   asm("_binary_node_key_pem_end");





static void https_get_request(struct esp_tls *tls)
{
    char buf[512];
    int ret, len;

    size_t written_bytes = 0;
    do {
        ret = esp_tls_conn_write(tls,
                                 REQUEST + written_bytes,
                                 sizeof(REQUEST) - written_bytes);
        if (ret >= 0) {
            ESP_LOGI(INFO_TAG, "%d bytes written", ret);
            written_bytes += ret;
        } else if (ret != ESP_TLS_ERR_SSL_WANT_READ  && ret != ESP_TLS_ERR_SSL_WANT_WRITE) {
            ESP_LOGE(INFO_TAG, "esp_tls_conn_write  returned: [0x%02X](%s)", ret, esp_err_to_name(ret));
            return;
        }
    } while (written_bytes < sizeof(REQUEST));

    ESP_LOGI(INFO_TAG, "Reading HTTP response...");

    do {
        len = sizeof(buf) - 1;
        bzero(buf, sizeof(buf));
        ret = esp_tls_conn_read(tls, (char *)buf, len);

        if (ret == ESP_TLS_ERR_SSL_WANT_WRITE  || ret == ESP_TLS_ERR_SSL_WANT_READ) {
            continue;
        }

        if (ret < 0) {
            ESP_LOGE(INFO_TAG, "esp_tls_conn_read  returned [-0x%02X](%s)", -ret, esp_err_to_name(ret));
            break;
        }

        if (ret == 0) {
            ESP_LOGI(INFO_TAG, "connection closed");
            break;
        }

        len = ret;
        ESP_LOGD(INFO_TAG, "%d bytes read", len);
        /* Print response directly to stdout as it is read */
        for (int i = 0; i < len; i++) {
            putchar(buf[i]);
        }
        putchar('\n'); // JSON output doesn't have a newline at end
    } while (1);
}

static void https_get_task(void *pvParameters)
{
    int repetitions = 3;
    while (repetitions > 0) {
        int conn_count = 0;
        ESP_LOGI(INFO_TAG, "");
        ESP_LOGI(INFO_TAG, "Connecting to %d URLs", MAX_URLS);
        for (int i = 0; i < MAX_URLS; i++) {
            esp_tls_cfg_t cfg = {
                .crt_bundle_attach = esp_crt_bundle_attach,
                .clientcert_buf  = (const unsigned char*) node_cert_pem_start,
                .clientcert_bytes = node_cert_pem_end - node_cert_pem_start,
                .clientkey_buf = (const unsigned char*) node_key_pem_start,
                .clientkey_bytes = node_key_pem_end - node_key_pem_start,
            };

            struct esp_tls *tls = esp_tls_conn_http_new(web_urls[i], &cfg);

            if (tls != NULL) {
                ESP_LOGI(INFO_TAG, "Connection established to %s", web_urls[i]);
                conn_count++;
                if (i == 0)
                    https_get_request(tls);
            } else {
                ESP_LOGE(INFO_TAG, "Could not connect to %s", web_urls[i]);
            }

            esp_tls_conn_delete(tls);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }

        ESP_LOGI(INFO_TAG, "Completed %d connections", conn_count);
        ESP_LOGI(INFO_TAG, "Starting over again...");
        repetitions -= 1;
    }
    ESP_LOGI(INFO_TAG, "Done");
    vTaskDelete(NULL);

}

void app_main(void)
{
    ESP_ERROR_CHECK( nvs_flash_init() );
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());

    ESP_LOGI(INFO_TAG, "Size of node cert: %d", node_cert_pem_end - node_cert_pem_start);
    ESP_LOGI(INFO_TAG, "Size of the key: %d", node_key_pem_end - node_key_pem_start);
    xTaskCreate(&https_get_task, "https_get_task", 8192, NULL, 5, NULL);
}
