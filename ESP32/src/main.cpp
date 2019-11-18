#include <FirebaseESP32.h>
#include <WiFi.h>

#include <Wire.h>
#include <BH1750.h>
#include <DHT.h>

#define FIREBASE_HOST "https://finalproject-35a1b.firebaseio.com"
#define FIREBASE_AUTH "603o4dr3kNDaNIfJotOhbN82cfMGbAOh9nJ21MPh"
#define WIFI_SSID "DIEGO"
#define WIFI_PASSWORD "MontyPython"

void setupInet();
void setupFirebase();
void setupSensors();
boolean lookupConfig();

void printResult(FirebaseData &data);

FirebaseData firebaseData;
FirebaseJson json,json2;
BH1750 lightMeter;
DHT dht;

// String workPath = "Edificios/DCIC/Labo4/Nodo1/Sensores";
String workPath = "";
String cfgPath = "config/nodos/";
String ip;
String mac;

String edificio,aula,nro_nodo;

float lux,temp,humidity;

boolean activeMode = 0;
boolean configOK = 0;

void setup(){

    Serial.begin(9600);
    while (!Serial) {
        ; // wait for serial port to connect.
    }

    setupInet();
    
    setupFirebase();
    
    setupSensors();

    Serial.println("*** Setup OK ***");
}

void loop() {

    if ((!edificio.isEmpty()) & (!aula.isEmpty()) & (!nro_nodo.isEmpty())) {
        workPath = "Edificios/" + edificio + "/" + aula + "/" + nro_nodo + "/Sensores";
        if (Firebase.pathExist(firebaseData,workPath)) {
            activeMode = 1;
        }
        else {
            activeMode = 0;
            lookupConfig();
        }
    } else {
        lookupConfig();
    }

    if (activeMode) {
        delay(500);
        delay(dht.getMinimumSamplingPeriod());
        
        lux = lightMeter.readLightLevel();
        lightMeter.configure(BH1750::ONE_TIME_HIGH_RES_MODE_2);
        delay(500);

        temp = dht.getTemperature();
        humidity = dht.getHumidity();

        Serial.print("\nLuminocidad\tHumedad\t\tTemperatura\n");
        Serial.print(lux);
        Serial.print(" lux");
        Serial.print("\t");
        Serial.print(humidity);
        Serial.print(" %");
        Serial.print("\t \t");
        Serial.print(temp);
        Serial.print(" C°");
        Serial.print("\n");

        if (Firebase.setFloat(firebaseData, workPath + "/Humedad", humidity)) {
            Serial.println("upload Humedad");
        } else {
            Serial.println("error al upload Humedad");
        }

        if (Firebase.setFloat(firebaseData, workPath + "/Temperatura", temp)) {
            Serial.println("upload Temperatura");
        } else {
            Serial.println("error al upload Temperatura");
        }

        if (Firebase.setFloat(firebaseData, workPath + "/Luminocidad", lux)) {
            Serial.println("upload Luminocidad");
        } else {
            Serial.println("error al upload Luminocidad");
        }

        delay(2500);
    } else {
        Serial.println("Esperando configuracion...");
        delay(1000 * 10); // esperando configuracion...
    }
    
}

void setupInet() {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(300);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    ip = WiFi.localIP().toString();
    Serial.println(ip);
    Serial.println();
    Serial.print("MAC Address: ");
    mac = WiFi.macAddress();
    Serial.println(mac);
}

void setupFirebase() {
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

    if (lookupConfig()) {
        Serial.println("Configuracion recuperada con exito");
    } else {
        Serial.println("Nodo no registrado... intentando registrar...");
        json.clear();
        json.add("edificio","");
        json.add("aula","");
        json.add("nro_nodo","");
        if (Firebase.setJSON(firebaseData,cfgPath + mac,json)) {
            Serial.println("Nodo registrado correctamente");
        }
    }
}

void setupSensors() {
    // Inicializacion sensor Luz
    Wire.begin(GPIO_NUM_22,GPIO_NUM_21);
    lightMeter.begin(BH1750::ONE_TIME_HIGH_RES_MODE_2);
    
    // Inicializacion Sensor Humedad y Tempreratura
    pinMode(GPIO_NUM_5,PULLUP);
    dht.setup(GPIO_NUM_5);
}

boolean lookupConfig() {
    if (Firebase.pathExist(firebaseData,cfgPath + mac)) {
        if (Firebase.getJSON(firebaseData,cfgPath + mac)) {
            // printResult(firebaseData);
            FirebaseJson &json = firebaseData.jsonObject();
            size_t len = json.iteratorBegin();
            String key, value = "";
            int type = 0;
            for (size_t i = 0; i < len; i++) {
                json.iteratorGet(i, type, key, value);
                if (key.equals("edificio")) 
                    edificio = value;
                else if (key.equals("aula"))
                    aula = value;
                else if (key.equals("nro_nodo"))
                    nro_nodo = value;
            }
            return true;
        }
    }
    return false;
}

void printResult(FirebaseData &data) {
    if (data.dataType() == "int")
        Serial.println(data.intData());
    else if (data.dataType() == "float")
        Serial.println(data.floatData(), 5);
    else if (data.dataType() == "double")
        printf("%.9lf\n", data.doubleData());
    else if (data.dataType() == "boolean")
        Serial.println(data.boolData() == 1 ? "true" : "false");
    else if (data.dataType() == "string")
        Serial.println(data.stringData());
    else if (data.dataType() == "json") {
        Serial.println();
        FirebaseJson &json = data.jsonObject();
        //Print all object data
        Serial.println("Pretty printed JSON data:");
        String jsonStr;
        json.toString(jsonStr, true);
        Serial.println(jsonStr);
        Serial.println();
        Serial.println("Iterate JSON data:");
        Serial.println();
        size_t len = json.iteratorBegin();
        String key, value = "";
        int type = 0;
        for (size_t i = 0; i < len; i++) {
            json.iteratorGet(i, type, key, value);
            Serial.print(i);
            Serial.print(", ");
            Serial.print("Type: ");
            Serial.print(type == JSON_OBJECT ? "object" : "array");
            if (type == JSON_OBJECT) {
                Serial.print(", Key: ");
                Serial.print(key);
            }
            Serial.print(", Value: ");
            Serial.println(value);
        }
        json.iteratorEnd();
    } else if (data.dataType() == "array") {
        Serial.println();
        //get array data from FirebaseData using FirebaseJsonArray object
        FirebaseJsonArray &arr = data.jsonArray();
        //Print all array values
        Serial.println("Pretty printed Array:");
        String arrStr;
        arr.toString(arrStr, true);
        Serial.println(arrStr);
        Serial.println();
        Serial.println("Iterate array values:");
        Serial.println();
        for (size_t i = 0; i < arr.size(); i++) {
            Serial.print(i);
            Serial.print(", Value: ");

            FirebaseJsonData &jsonData = data.jsonData();
            //Get the result data from FirebaseJsonArray object
            arr.get(jsonData, i);
            if (jsonData.typeNum == JSON_BOOL)
                Serial.println(jsonData.boolValue ? "true" : "false");
            else if (jsonData.typeNum == JSON_INT)
                Serial.println(jsonData.intValue);
            else if (jsonData.typeNum == JSON_DOUBLE)
                printf("%.9lf\n", jsonData.doubleValue);
            else if (jsonData.typeNum == JSON_STRING ||
                     jsonData.typeNum == JSON_NULL ||
                     jsonData.typeNum == JSON_OBJECT ||
                     jsonData.typeNum == JSON_ARRAY)
                Serial.println(jsonData.stringValue);
        }
    }
}