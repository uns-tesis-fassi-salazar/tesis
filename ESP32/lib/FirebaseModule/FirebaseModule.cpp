#include <FirebaseModule.h>

#define FIREBASE_HOST "https://finalproject-35a1b.firebaseio.com"
#define FIREBASE_AUTH "603o4dr3kNDaNIfJotOhbN82cfMGbAOh9nJ21MPh"

FirebaseData firebaseData;
FirebaseJson json, json2;

uint32_t logIndex = 0;
uint32_t secuencial = 0;

void setUpFirebase(const char *fVersion)
{
    if (WiFi.status() == WL_CONNECTED)
    {

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

        if (Firebase.pathExist(firebaseData, NODOS + WiFi.macAddress() + "/"))
        {
            uploadLogs("Nodo ya registrado en Firebase.");
        }
        else
        {
            uploadLogs("Nodo no registrado en Firebase... intentando registrar...");
            json.clear();
            // json.add("aulaAsignada","");
            json.add("firmwareVersion", fVersion);
            json.add("comando", 0);
            // SENSORES
            json2.clear();
            json2.add("luminocidad", 0);
            json2.add("temperatura", 0);
            json2.add("humedad", 0);
            json2.add("movimiento", 0);
            json2.add("corriente", 0);
            json.add("sensores", json2);
            if (Firebase.setJSON(firebaseData, NODOS + WiFi.macAddress() + "/", json))
            {
                uploadLogs("Nodo registrado correctamente en Firebase");
                Serial.println("Nodo registrado correctamente en Firebase");
            }
        }
    }
}

void setStreamCallback(FirebaseData &fbDataStream, String dbPath, StreamEventCallback eventCallBack, StreamTimeoutCallback timeoutCallback)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        if (!Firebase.beginStream(fbDataStream, dbPath))
        {
            uploadLogs(fbDataStream.errorReason());
            uploadLogs("No pude beginStream: " + dbPath);
        }
        Firebase.setStreamCallback(fbDataStream, eventCallBack, timeoutCallback);
    }
}

void removeStreamCallback(FirebaseData &fbDataStream)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        Firebase.removeStreamCallback(fbDataStream);
    }
}

bool pauseStreamCallback(FirebaseData &fbDataStream)
{
    return Firebase.endStream(fbDataStream);
}

bool restoreStreamCallback(FirebaseData &fbDataStream, const String dbPath)
{
    return Firebase.beginStream(fbDataStream, dbPath);
}

String getAulaAsignada()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        if (Firebase.getString(firebaseData, NODOS + WiFi.macAddress() + "/" + AULA_ASIGNADA))
        {
            return firebaseData.stringData();
        }
    }
    return "";
}

bool getAulaConfig(struct aulaConfig *aulaCnf, String aulaKey)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        String cnfAux;
        if (Firebase.getString(firebaseData, AULAS + aulaKey + "/horaInicioAuto"))
        {
            cnfAux = firebaseData.stringData();
            for (int i = 0; i < cnfAux.length(); i++)
            {
                if (cnfAux.substring(i, i + 1) == ":")
                {
                    aulaCnf->horaIniAuto = cnfAux.substring(0, i).toInt();
                    aulaCnf->minIniAuto = cnfAux.substring(i + 1).toInt();
                    break;
                }
            }
        }
        else
        {
            Serial.println("falla horaInicioAuto");
            return false;
        }
        if (Firebase.getString(firebaseData, AULAS + aulaKey + "/horaFinAuto"))
        {
            cnfAux = firebaseData.stringData();
            for (int i = 0; i < cnfAux.length(); i++)
            {
                if (cnfAux.substring(i, i + 1) == ":")
                {
                    aulaCnf->horaFinAuto = cnfAux.substring(0, i).toInt();
                    aulaCnf->minFinAuto = cnfAux.substring(i + 1).toInt();
                    break;
                }
            }
        }
        else
        {
            Serial.println("falla horaFinAuto");
            return false;
        }
        if (Firebase.getInt(firebaseData, AULAS + aulaKey + "/timeoutAulaVacia"))
        {
            aulaCnf->timeOutAulaVacia = firebaseData.intData();
        }
        else
        {
            Serial.println("falla timeoutAulaVacia");
            return false;
        }
        if (Firebase.getInt(firebaseData, AULAS + aulaKey + "/intervaloLecturas"))
        {
            aulaCnf->intervaloLecturas = firebaseData.intData();
        }
        else
        {
            Serial.println("falla intervaloLecturas");
            return false;
        }
        if (Firebase.getInt(firebaseData, AULAS + aulaKey + "/zeroSensorHall"))
        {
            aulaCnf->zeroSensorHall = firebaseData.intData();
        }
        else
        {
            Serial.println("falla zeroSensorHall");
            return false;
        }
        if (Firebase.getFloat(firebaseData, AULAS + aulaKey + "/umbralSensorHall"))
        {
            aulaCnf->umbralSensorHall = firebaseData.floatData();
        }
        else
        {
            Serial.println("falla umbralSensorHall");
            return false;
        }
        aulaCnf->hasConfig = true;
        return true;
    }
    return false;
}

String getFirmwareVersion()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        if (Firebase.getString(firebaseData, NODOS + WiFi.macAddress() + "/" + DB_FIRMWARE_VERSION))
        {
            return firebaseData.stringData();
        }
    }
    return "";
}

bool updateFirmwareVersion(const char *fVersion)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        return Firebase.setString(firebaseData, NODOS + WiFi.macAddress() + "/" + DB_FIRMWARE_VERSION, fVersion);
    }
    return false;
}

String getFirmwareURL(String new_ver)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        if (Firebase.getString(firebaseData, FIRMWARE + new_ver + "/" + URL))
        {
            return firebaseData.stringData();
        }
    }
    return "";
}

bool readData(String path, int *value)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        if (Firebase.getInt(firebaseData, NODOS + WiFi.macAddress() + "/" + path))
        {
            *value = firebaseData.intData();
            return true;
        }
    }
    return false;
}

bool readActuador(String actuadorId, int *value)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        if (Firebase.getInt(firebaseData, NODOS + WiFi.macAddress() + "/" + ACTUADORES + actuadorId))
        {
            *value = firebaseData.intData();
            return true;
        }
    }
    return false;
}

bool uploadData(FirebaseJson &json)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        return Firebase.updateNodeSilent(firebaseData, NODOS + WiFi.macAddress() + "/" + SENSORES, json);
    }
    return false;
}

bool uploadData(float value, String sensorId)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        if (!isnan(value))
        {
            return Firebase.setFloat(firebaseData, NODOS + WiFi.macAddress() + "/" + SENSORES + sensorId, value);
        }
    }
    return false;
}

bool uploadData(int value, String sensorId)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        if (!isnan(value))
        {
            return Firebase.setInt(firebaseData, NODOS + WiFi.macAddress() + "/" + SENSORES + sensorId, value);
        }
    }
    return false;
}

String createKeyCommand()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        if (Firebase.pushTimestamp(firebaseData, COMANDOS_IR))
        {
            return firebaseData.pushName();
        }
    }
    return "";
}

String getKeyCommand(String pathKey)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        if (Firebase.getString(firebaseData, AULAS + pathKey + "/" + COMANDO_IR))
        {
            return firebaseData.stringData();
        }
    }
    return "";
}

bool uploadKeyCommand(String pathKey, String commandKey)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        return Firebase.setString(firebaseData, AULAS + pathKey + "/" + COMANDO_IR, commandKey);
    }
    return false;
}

bool deleteKeyCommand(String commandKey)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        return Firebase.deleteNode(firebaseData, COMANDOS_IR + commandKey);
    }
    return false;
}

bool getBlobData(String pathKey, std::vector<uint8_t> *data)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        if (Firebase.getBlob(firebaseData, COMANDOS_IR + pathKey))
        {
            if (firebaseData.dataType() == "blob")
            {
                *data = firebaseData.blobData();
                return true;
            }
        }
    }
    return false;
}

bool uploadBlobData(String pathKey, uint8_t *data, int length)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        return Firebase.setBlob(firebaseData, COMANDOS_IR + pathKey, data, length * sizeof(uint8_t));
    }
    return false;
}

bool setCommandValue(String pathKey, String value)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        return Firebase.setString(firebaseData, COMANDOS_IR + pathKey, value);
    }
    return false;
}

bool uploadLogs(String log)
{
    secuencial = getUIntFromNVM("secuencial");
    logIndex = getUIntFromNVM("logIndex");
    if (WiFi.status() == WL_CONNECTED)
    {
        json.clear();
        json.add("fecha", tiempoCortoToString());
        json.add("secuencial", (int)secuencial);
        json.add("contenido", log);

        if (Firebase.setJSON(firebaseData, LOGS + WiFi.macAddress() + "/" + logIndex, json))
        {
            secuencial++;
            logIndex++;
            if (logIndex > 4999)
            {
                logIndex = 0;
            }
            if (secuencial >= 0xFFFFFFFA)
            {
                secuencial = 0;
            }
            putUIntInNVM("secuencial", secuencial);
            putUIntInNVM("logIndex", logIndex);
            return true;
        }
    }
    return false;
}