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
#include "nvs.h"

#include <stdio.h>
#include "soc/efuse_reg.h"
#include "esp_efuse.h"
#include "esp_spi_flash.h"
#include "esp_partition.h"
#include "esp_flash_encrypt.h"
#include "esp_efuse_table.h"



// Flash encryption constants
static const char* TAG = "Hexdump";
#if CONFIG_IDF_TARGET_ESP32
#define TARGET_CRYPT_CNT_EFUSE  ESP_EFUSE_FLASH_CRYPT_CNT
#define TARGET_CRYPT_CNT_WIDTH  7
#elif CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32C3
#define TARGET_CRYPT_CNT_EFUSE ESP_EFUSE_SPI_BOOT_CRYPT_CNT
#define TARGET_CRYPT_CNT_WIDTH  3
#endif

// HTTPS constants
/* Constants that aren't configurable in menuconfig */
#define WEB_SERVER "192.168.0.12"
#define WEB_PORT "9999"
#define WEB_URL "https://192.168.0.12:9999/authenticate"
// #define WEB_URL "https://192.168.0.12:9999/"

static const char REQUEST[] = "GET " WEB_URL " HTTP/1.1\r\n"
                             "Host: "WEB_SERVER"\r\n"
                             "User-Agent: esp-idf/1.0 esp32\r\n"
                             "\r\n";

#define MAX_URLS    3

static const char *web_urls[MAX_URLS] = {
    // "https://aulas-sustentables.herokuapp.com",
    "https://github.com",
    "https://firebase.google.com",
    "https://192.168.0.12:9999",
};

static const char *INFO_TAG = "info";

typedef struct client_certs
{
    char* cert;
    unsigned int cert_len;
    char* priv_key;
    unsigned int priv_key_len;
} client_certs_t;


client_certs_t g_client_certs;


// extern const uint8_t node_cert_pem_start[] asm("_binary_node_cert_pem_start");
// extern const uint8_t node_cert_pem_end[]   asm("_binary_node_cert_pem_end");

// extern const uint8_t node_key_pem_start[] asm("_binary_node_key_pem_start");
// extern const uint8_t node_key_pem_end[]   asm("_binary_node_key_pem_end");


esp_err_t nvs_secure_initialize() {
    static const char *nvs_tag = "nvs";
    esp_err_t err = ESP_OK;

    // 1. find partition with nvs_keys
    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA,
                                                                ESP_PARTITION_SUBTYPE_DATA_NVS_KEYS,
                                                                "nvs_key");
    if (partition == NULL) {
        ESP_LOGE(nvs_tag, "Could not locate nvs_key partition. Aborting.");
        return ESP_FAIL;
    }

    // 2. read nvs_keys from key partition
    nvs_sec_cfg_t cfg;
    if (ESP_OK != (err = nvs_flash_read_security_cfg(partition, &cfg))) {
        ESP_LOGE(nvs_tag, "Failed to read nvs keys (rc=0x%x)", err);
        return err;
    }

    // 3. initialize nvs partition
    if (ESP_OK != (err = nvs_flash_secure_init(&cfg))) {
        ESP_LOGE(nvs_tag, "failed to initialize nvs partition (err=0x%x). Aborting.", err);
        return err;
    };

    return err;
}

char* nvs_get_string(char* key, nvs_handle_t nvs_handle)
{
    esp_err_t err;
    size_t len;
    err = nvs_get_str(nvs_handle, key, NULL, &len);
    switch (err) {
        case ESP_OK:
            printf("nvs[%s] -> ", key);
            char* str = (char *)malloc(len);
            if ( (err = nvs_get_str(nvs_handle, key, str, &len)) == ESP_OK) {
                printf("%s\n", str);
            } else {
                printf("Error (%s) reading!\n", esp_err_to_name(err));
            }
            // free(str);
            return str;
            // break;
        case ESP_ERR_NVS_NOT_FOUND:
            printf("nvs[%s] -> Value not found!\n", key);
            break;
        default :
            printf("Error (%s) reading!\n", esp_err_to_name(err));
    }
    return NULL;
}

void nvs_get_client_cert_key(client_certs_t* client_certs)
{
    esp_err_t err;
    
    // Open
    printf("\n");
    printf("Opening Non-Volatile Storage (NVS) handle... ");
    nvs_handle_t my_handle;
    err = nvs_open("crypto", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } else {
        printf("Done\n");
    }

    // Read stats
    nvs_stats_t nvs_stats;
    err = nvs_get_stats(NULL, &nvs_stats);
    switch (err) {
        case ESP_OK:
            printf("Count: UsedEntries = (%d), FreeEntries = (%d), AllEntries = (%d)\n", nvs_stats.used_entries, nvs_stats.free_entries, nvs_stats.total_entries);
            break;
        case ESP_ERR_NVS_NOT_FOUND:
            printf("Value not found!\n");
            break;
        default :
            printf("Error (%s) reading!\n", esp_err_to_name(err));
    }

    // Read
    client_certs->priv_key = nvs_get_string("clientKey", my_handle);
    client_certs->priv_key_len = strlen(client_certs->priv_key) + 1;
    client_certs->cert = nvs_get_string("clientCert", my_handle);
    client_certs->cert_len = strlen(client_certs->cert) + 1;
    
    // Close
    nvs_close(my_handle);
}


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
                .clientcert_buf  = (const unsigned char*) g_client_certs.cert,
                .clientcert_bytes = g_client_certs.cert_len,
                .clientkey_buf = (const unsigned char*) g_client_certs.priv_key,
                .clientkey_bytes = g_client_certs.priv_key_len,
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


static void example_print_chip_info(void)
{
    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is %s chip with %d CPU core(s), WiFi%s%s, ",
            CONFIG_IDF_TARGET,
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
}


static void example_print_flash_encryption_status(void)
{
    uint32_t flash_crypt_cnt = 0;
    esp_efuse_read_field_blob(TARGET_CRYPT_CNT_EFUSE, &flash_crypt_cnt, TARGET_CRYPT_CNT_WIDTH);
    printf("\nFLASH_CRYPT_CNT eFuse value is %d\n", flash_crypt_cnt);

    esp_flash_enc_mode_t mode = esp_get_flash_encryption_mode();
    if (mode == ESP_FLASH_ENC_MODE_DISABLED) {
        printf("Flash encryption feature is disabled\n");
    } else {
        printf("Flash encryption feature is enabled in %s mode\n",
            mode == ESP_FLASH_ENC_MODE_DEVELOPMENT ? "DEVELOPMENT" : "RELEASE");
    }
}


static void example_read_write_flash(void)
{
    const esp_partition_t* partition = esp_partition_find_first(
        ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage");
    assert(partition);

    printf("\nErasing partition \"%s\" (0x%x bytes)\n", partition->label, partition->size);

    ESP_ERROR_CHECK(esp_partition_erase_range(partition, 0, partition->size));

    /* Generate the data which will be written */
    const size_t data_size = 32;
    uint8_t plaintext_data[data_size];
    for (uint8_t i = 0; i < data_size; ++i) {
        plaintext_data[i] = i;
    }

    printf("Writing data with esp_partition_write:\n");
    ESP_LOG_BUFFER_HEXDUMP(TAG, plaintext_data, data_size, ESP_LOG_INFO);
    ESP_ERROR_CHECK(esp_partition_write(partition, 0, plaintext_data, data_size));

    uint8_t read_data[data_size];
    printf("Reading with esp_partition_read:\n");
    ESP_ERROR_CHECK(esp_partition_read(partition, 0, read_data, data_size));
    ESP_LOG_BUFFER_HEXDUMP(TAG, read_data, data_size, ESP_LOG_INFO);

    printf("Reading with spi_flash_read:\n");
    ESP_ERROR_CHECK(spi_flash_read(partition->address, read_data, data_size));
    ESP_LOG_BUFFER_HEXDUMP(TAG, read_data, data_size, ESP_LOG_INFO);
}

void https_request()
{
    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());
    // ESP_LOGI(INFO_TAG, "Size of node cert: %d", node_cert_pem_end - node_cert_pem_start);
    // ESP_LOGI(INFO_TAG, "Size of the key: %d", node_key_pem_end - node_key_pem_start);
    xTaskCreate(&https_get_task, "https_get_task", 8192, NULL, 5, NULL);
}

void nvs_encryption()
{
    esp_err_t err = nvs_secure_initialize();
    if (err != ESP_OK) {
        ESP_LOGE("main", "Failed to initialize nvs (rc=0x%x). Halting.", err);
        while(1) { vTaskDelay(100); }
    }
    // client_certs_t client_certs;
    nvs_get_client_cert_key(&g_client_certs);
    printf("Client certs from struct. Key len: %d; cert len: %d\n\n\n", g_client_certs.priv_key_len, g_client_certs.cert_len);
}



void flash_encryption()
{
    printf("\nExample to check Flash Encryption status:\n");

    example_print_chip_info();
    example_print_flash_encryption_status();
    example_read_write_flash();
}

void app_main(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    flash_encryption();
    nvs_encryption();
    https_request();
    while(1) { vTaskDelay(100); }
}
