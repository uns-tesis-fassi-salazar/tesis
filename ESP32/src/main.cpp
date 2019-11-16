#include <FirebaseESP32.h>
#include <WiFi.h>

#include <Wire.h>
#include <BH1750.h>
#include <DHT.h>

#define FIREBASE_HOST "https://finalproject-35a1b.firebaseio.com"
#define FIREBASE_AUTH "603o4dr3kNDaNIfJotOhbN82cfMGbAOh9nJ21MPh"
#define WIFI_SSID "DIEGO"
#define WIFI_PASSWORD "MontyPython"

FirebaseData firebaseData;
void printResult(FirebaseData &data);

BH1750 lightMeter;
DHT dht;

String path = "/DCIC/Labo4/Nodo1/Sensores";

void setup(){

    Serial.begin(9600);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB port only
    }
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(300);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();

    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    Firebase.reconnectWiFi(true);

    //Set database read timeout to 1 minute (max 15 minutes)
    Firebase.setReadTimeout(firebaseData, 1000 * 60);
    //tiny, small, medium, large and unlimited.
    //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
    Firebase.setwriteSizeLimit(firebaseData, "tiny");

    /*
    This option allows get and delete functions (PUT and DELETE HTTP requests) works for device connected behind the
    Firewall that allows only GET and POST requests.
    
    Firebase.enableClassicRequest(firebaseData, true);
    */
    FirebaseJson json;

    // Inicializacion sensor Luz
    Wire.begin(GPIO_NUM_22,GPIO_NUM_21);
    lightMeter.begin(BH1750::ONE_TIME_HIGH_RES_MODE_2);
    
    // Inicializacion Sensor Humedad y Tempreratura
    pinMode(GPIO_NUM_5,PULLUP);
    dht.setup(GPIO_NUM_5);

    Serial.println("*** Setup OK ***");
}

float lux,temp,humidity;

void loop() {

    delay(500);
    delay(dht.getMinimumSamplingPeriod());
    
    lux = lightMeter.readLightLevel();
    Serial.print("\nLight: ");
    Serial.print(lux);
    Serial.println(" lx");
    lightMeter.configure(BH1750::ONE_TIME_HIGH_RES_MODE_2);
    delay(500);

    Serial.print("Humedad\t Temperatura\n");

    temp = dht.getTemperature();
    humidity = dht.getHumidity();

    Serial.print(humidity);
    Serial.print("\t");
    Serial.print(temp);
    Serial.print("\n");

    if (Firebase.setFloat(firebaseData, path + "/Humedad", humidity)) {
        Serial.println("upload Humedad");
    } else {
        Serial.println("error al upload Humedad");
    }

    if (Firebase.setFloat(firebaseData, path + "/Temperatura", temp)) {
        Serial.println("upload Temperatura");
    } else {
        Serial.println("error al upload Temperatura");
    }

    if (Firebase.setFloat(firebaseData, path + "/Luminocidad", lux)) {
        Serial.println("upload Luminocidad");
    } else {
        Serial.println("error al upload Luminocidad");
    }

    delay(2500);
}