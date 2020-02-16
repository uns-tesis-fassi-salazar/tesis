#include <Sensores.h>

void logSensores();

FirebaseJson jsonSensorData;

ACS712 hallSensor(ACS712_20A, HALL_PIN);
BH1750 lightMeter;
DHT dht;

float prevLuxValue, currentLuxValue;
float prevTempValue, currentTempValue;
float prevHumidityValue, currentHumidityValue;
float prevHallValue, currentHallValue;

ulong tiempoUltimaLecturaDHT = 0;

struct Movement {
    const uint8_t PIN;
    bool emptyRoom;
    bool roomStateChange;
};

hw_timer_t *movementTimer = NULL;

Movement movement = {MOVEMENT_PIN, true, false};

void IRAM_ATTR aulaVacia() {
    movement.roomStateChange = !movement.emptyRoom;
    movement.emptyRoom = true;
}

void IRAM_ATTR movementDetection() {
    movement.roomStateChange = movement.emptyRoom; 
    movement.emptyRoom = false;
    timerRestart(movementTimer);
}

bool emptyRoomState() {
    return movement.emptyRoom;
}

void setUpSensors() {
    prevLuxValue = 0;
    prevTempValue = 0;
    prevHumidityValue = 0;
    prevHallValue = 0;
    // Inicializacion Sensor ACS712
    // If you are not sure that the current through the sensor will not leak during calibration - comment out this method
    uploadLogs("Calibrating... Ensure that no current flows through the sensor at this moment");
    uploadLogs("Valor calibrado: " + hallSensor.calibrate());
    Serial.println("Valor calibrado: " + hallSensor.calibrate());

    // Inicializacion sensor Luz
    Wire.begin(LUX_SDA_PIN, LUX_CSL_PIN);
    lightMeter.begin(BH1750::ONE_TIME_HIGH_RES_MODE_2);

    // Inicializacion Sensor Humedad y Tempreratura
    pinMode(DHT_PIN, PULLUP);
    dht.setup(DHT_PIN);

    // Set movement sensor timer
    int timeOutValue = 0;
    if (hasAulaConfig()) {
        timeOutValue = getTimeoutAulaVacia();
    } else {
        timeOutValue = 60;
    }
    movementTimer = timerTimeout(timeOutValue*1000, aulaVacia);

    // Inicializacion Sensor Movimiento
    pinMode(MOVEMENT_PIN, PULLUP);
    attachInterrupt(movement.PIN, movementDetection, HIGH);
}

float readLuxValue() {
    // delay(dht.getMinimumSamplingPeriod());
    lightMeter.configure(BH1750::ONE_TIME_HIGH_RES_MODE_2);
    currentLuxValue = truncToDec(lightMeter.readLightLevel(),2);
    return currentLuxValue;
}

void readSensorsValues() {
    currentHallValue = hallSensor.getCurrentAC(50);
    Serial.printf("Hall: %f\n", currentHallValue);
    currentLuxValue = readLuxValue();
    
    if (lapTimer(2500, &tiempoUltimaLecturaDHT)) {
        currentTempValue = truncToDec(dht.getTemperature(),2);
        currentHumidityValue = truncToDec(dht.getHumidity(),2);
    }
}

void uploadSensorsValues() {
    if ((prevTempValue != currentTempValue) || 
        (prevHumidityValue != currentHumidityValue) || 
        (prevLuxValue != currentLuxValue) || 
        (prevHallValue != currentHallValue) ||
        movement.roomStateChange) {
            // printSensors();
            logSensores();

            jsonSensorData.clear();
            jsonSensorData.add("luminocidad", (double)currentLuxValue);
            if (!isnan(currentTempValue) && !isnan(currentHumidityValue)) {
                jsonSensorData.add("temperatura", (double)currentTempValue);
                jsonSensorData.add("humedad", (double)currentHumidityValue);
            }
            // jsonSensorData.add("corriente", (double)currentHallValue);
            jsonSensorData.add("movimiento", !(double)movement.emptyRoom);
            if (!uploadData(jsonSensorData)) {
                // Serial.println("Sensor JSON NOT OK");
                uploadLogs("Sensor JSON NOT OK");
            }

            prevLuxValue = currentLuxValue;
            prevTempValue = currentTempValue;
            prevHumidityValue = currentHumidityValue;
            prevHallValue = currentHallValue;
            movement.roomStateChange = false;
    }
}

bool hasCurrentFlow() {
    if (currentHallValue > 2.50) {
        return true;
    }
    return false;
}

void setTimerTimeout(int timeout) {
    movementTimer = timerTimeout(timeout*1000, aulaVacia);
}

void printSensors() {
    Serial.print("\nLuminocidad\tCorriente\tHumedad\t\tTemperatura\tAula Vacia\n");
    Serial.print(currentLuxValue);
    Serial.print(" lux");
    Serial.print("\t");
    // Serial.print(currentHallValue);
    // Serial.print(" A");
    Serial.print("\t\t");
    Serial.print(currentHumidityValue);
    Serial.print(" %");
    Serial.print("\t \t");
    Serial.print(currentTempValue);
    Serial.print(" C°");
    Serial.print("\t\t");
    Serial.print(movement.emptyRoom);
    Serial.print("\n");
}

void logSensores() {
    String toLog;
    toLog += "Lux: " + String(currentLuxValue);
    // toLog += " - I: " + String(currentHallValue) + " A";
    toLog += " - Hum: " + String(currentHumidityValue) + " %";
    toLog += " - Temp: " + String(currentTempValue) + " C°";
    toLog += " - Aula Vacia: " + String(movement.emptyRoom);
    
    uploadLogs(toLog);
}

void loopSensors() {
    readSensorsValues();
    uploadSensorsValues();
}