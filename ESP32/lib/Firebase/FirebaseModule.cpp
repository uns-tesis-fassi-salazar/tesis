#include <FirebaseModule.h>

#define FIREBASE_HOST "https://finalproject-35a1b.firebaseio.com"
#define FIREBASE_AUTH "603o4dr3kNDaNIfJotOhbN82cfMGbAOh9nJ21MPh"

FirebaseData firebaseData;
FirebaseJson json,json2;

void setUpFirebase() {

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

    if (Firebase.pathExist(firebaseData,NODOS + WiFi.macAddress() + "/")) {
        Serial.println("Nodo registrado.");
    } else {
        Serial.println("Nodo no registrado... intentando registrar...");
        json.clear();
        json.add("aulaAsignada","");
        json.add("firmwareVersion","");
        // SENSORES
        json2.clear();
        json2.add("aire",0);
        json2.add("luces",0);
        json2.add("proyector",0);
        json.add("actuadores",json2);
        // ACTUADORES
        json2.clear();
        json2.add("luminocidad",0);
        json2.add("temperatura",0);
        json2.add("humedad",0);
        json2.add("movimiento",0);
        json.add("sensores",json2);
        // // CONFIGURACION
        json2.clear();
        json2.add("tiempoVacia",10);
        json2.add("tiempoEntreLecturas",0);
        json.add("configuracion",json2);
        if (Firebase.setJSON(firebaseData,NODOS + WiFi.macAddress() + "/",json)) {
            Serial.println("Nodo registrado correctamente");
        }
    }
}

void setStreamCallback(FirebaseData &fbDataStream,String dbPath,StreamEventCallback eventCallBack, StreamTimeoutCallback timeoutCallback) {
    if (!Firebase.beginStream(fbDataStream, dbPath))
    {
        Serial.println(fbDataStream.errorReason());
        Serial.print("No pude beginStream: ");
        Serial.println(dbPath);
    }
    Firebase.setStreamCallback(fbDataStream, eventCallBack, timeoutCallback);
}

void removeStreamCallback(FirebaseData &fbDataStream) {
    Firebase.removeStreamCallback(fbDataStream);
}

String getAulaAsignada() {
    if (Firebase.getString(firebaseData,NODOS + WiFi.macAddress() + "/" + AULA_ASIGNADA)) {
        return firebaseData.stringData();
    } else return "";
}

String getFirmwareVersion() {
    if (Firebase.getString(firebaseData,NODOS + WiFi.macAddress() + "/" + DB_FIRMWARE_VERSION)) {
        return firebaseData.stringData();
    } else return "";
}

boolean updateFirmwareVersion(const char *fVersion) {
    return Firebase.setString(firebaseData,NODOS + WiFi.macAddress() + "/" + DB_FIRMWARE_VERSION,fVersion);
}

String getFirmwareURL(String new_ver) {
    if (Firebase.getString(firebaseData,FIRMWARE + new_ver + "/" + URL)) {
        return firebaseData.stringData();
    } else return "";
}

boolean readData(String path, int *value) {
    if (Firebase.getInt(firebaseData,NODOS + WiFi.macAddress() + "/" + path)) {
        *value = firebaseData.intData();
        return true;
    } else return false;
}

boolean readActuador(String actuadorId, int *value) {
    if (Firebase.getInt(firebaseData,NODOS + WiFi.macAddress() + "/" + ACTUADORES + actuadorId)) {
        *value = firebaseData.intData();
        return true;
    } else return false;
}

boolean uploadData(FirebaseJson &json) {
    return Firebase.setJSON(firebaseData, NODOS + WiFi.macAddress() + "/" + SENSORES, json);
}

boolean uploadData(float value, String sensorId) {
    if (!isnan(value)) {
        return Firebase.setFloat(firebaseData, NODOS + WiFi.macAddress() + "/" + SENSORES + sensorId, value);
    } else return false;
}

boolean uploadData(int value, String sensorId) {
    if (!isnan(value)) {
        return Firebase.setInt(firebaseData, NODOS + WiFi.macAddress() + "/" + SENSORES + sensorId, value);
    } else return false;
}

String createKeyCommand() {
    if (Firebase.pushTimestamp(firebaseData,COMANDOS_IR)) {
        return firebaseData.pushName();
    } return "";
}

String getKeyCommand(String pathKey) {
    if (Firebase.getString(firebaseData,AULAS + pathKey + COMANDOS_IR)) {
        return firebaseData.stringData();
    } return "";
}

boolean uploadBlobData(String pathKey, uint8_t * data, int length) {
    return Firebase.setBlob(firebaseData, COMANDOS_IR + pathKey, data, length * sizeof(uint8_t));
}

void getBlobData(String pathKey, std::vector<uint8_t> * data) {
    if (Firebase.getBlob(firebaseData, COMANDOS_IR + pathKey)) {
        if (firebaseData.dataType() == "blob") {
            *data = firebaseData.blobData();
        }
    }
}