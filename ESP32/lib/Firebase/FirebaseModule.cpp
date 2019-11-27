#include "FirebaseModule.h"

#define FIREBASE_HOST "https://finalproject-35a1b.firebaseio.com"
#define FIREBASE_AUTH "603o4dr3kNDaNIfJotOhbN82cfMGbAOh9nJ21MPh"

FirebaseData firebaseData;
FirebaseJson json,json2;

String mac;

#define nodePath "Nodos/"

void setupFirebase(String nodoMac) {

    mac = nodoMac + "/";

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

boolean readData(String path, int *value) {
    if (Firebase.getInt(firebaseData,nodePath + mac + path)) {
        *value = firebaseData.intData();
        return true;
    } else return false;
}

boolean readActuador(String actuadorId, int *value) {
    if (Firebase.getInt(firebaseData,nodePath + mac + "Actuadores/" + actuadorId)) {
        *value = firebaseData.intData();
        return true;
    } else return false;
}

boolean uploadData(FirebaseJson &jsonData) {
    return Firebase.setJSON(firebaseData, nodePath + mac + "Sensores/", jsonData);
}

boolean uploadData(float value,String sensorId) {
    if (!isnan(value)) {
        return Firebase.setFloat(firebaseData, nodePath + mac + "Sensores/" + sensorId, value);
    } else return false;
}

boolean uploadData(int value,String sensorId) {
    if (!isnan(value)) {
        return Firebase.setInt(firebaseData, nodePath + mac + "Sensores/" + sensorId, value);
    } else return false;
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