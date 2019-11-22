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
boolean lookupActiveMode();
void printTittle();
void uploadData();
void printFirebaseResult(FirebaseData &data);

FirebaseData firebaseData;
FirebaseJson json,json2;
BH1750 lightMeter;
DHT dht;

String nodePath = "Nodos/";
String ip;
String mac;

float lux,temp,humidity;

boolean activeMode = 0;
boolean secondsToSleep = 10;

void setup(){

    Serial.begin(9600);
    while (!Serial) {
        ; // wait for serial port to connect.
    }

    setupInet();
    
    setupFirebase();
    
    if (mac != "CC:50:E3:B6:29:48") {
        setupSensors();
    }

    Serial.println("*** Setup OK ***");
}

void loop() {

    if (lookupActiveMode() ? activeMode = 1 : activeMode = 0);

    if (activeMode) {
        delay(500);
        if (mac != "CC:50:E3:B6:29:48") {
            delay(dht.getMinimumSamplingPeriod());
            
            lux = lightMeter.readLightLevel();
            lightMeter.configure(BH1750::ONE_TIME_HIGH_RES_MODE_2);
            delay(500);

            temp = dht.getTemperature();
            humidity = dht.getHumidity();
        } else {
            temp = temperatureRead();
            humidity = 0;
            lux = 0;
        }

        printTittle();

        uploadData();

        delay(2500);

    } else {
        Serial.println("Esperando asignacion de aula...");
        delay(1000 * secondsToSleep); // esperando configuracion...
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

    if (Firebase.pathExist(firebaseData,nodePath + mac)) {
        Serial.println("Nodo registrado.");
    } else {
        Serial.println("Nodo no registrado... intentando registrar...");
        json.clear();
        json.add("AulaAsignada",0);
        json2.clear();
        json2.add("Aire",0);
        json2.add("Luces",0);
        json2.add("Proyector",0);
        json.add("Actuadores",json2);
        json2.clear();
        json2.add("Luminocidad",0);
        json2.add("Temperatura",0);
        json2.add("Humedad",0);
        json2.add("Movimiento",0);
        json.add("Sensores",json2);
        if (Firebase.setJSON(firebaseData,nodePath + mac,json)) {
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

boolean lookupActiveMode() {
    if (Firebase.pathExist(firebaseData,nodePath + mac)) {
        if (Firebase.getInt(firebaseData,nodePath + mac + "/AulaAsignada")) {
            if (firebaseData.intData() == 0) {
                return false;
            } else {
                return true;
            }
        } else {
            Serial.println("No puedo recuperar el valor AulaAsignada");
            return false;
        }
    } else {
        Serial.println("No existe el path");
        return false;
    }
}

void printTittle() {
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
}

void uploadData() {
    if (!isnan(humidity)) {
        if (Firebase.setFloat(firebaseData, nodePath + mac + "/Sensores/Humedad", humidity) ? Serial.println("upload Humedad") : Serial.println("error al upload Humedad"));
    }

    if (!isnan(temp)) {
        if (Firebase.setFloat(firebaseData, nodePath + mac + "/Sensores/Temperatura", temp) ? Serial.println("upload Temperatura") : Serial.println("error al upload Temperatura"));
    }

    if (!isnan(lux)) {
        if (Firebase.setFloat(firebaseData, nodePath + mac + "/Sensores/Luminocidad", lux) ? Serial.println("upload Luminocidad") : Serial.println("error al upload Luminocidad"));
    }
}

void printFirebaseResult(FirebaseData &data) {
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