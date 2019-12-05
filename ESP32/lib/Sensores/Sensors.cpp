#include <Sensors.h>
#include <FirebaseModule.h>

#define SENSOR_HUMEDAD "Humedad"
#define SENSOR_TEMPERATURA "Temperatura"
#define SENSOR_LUX "Luminocidad"
#define SENSOR_MOV "Movimiento"

#define MOVEMENT_PIN GPIO_NUM_25
#define LUX_SDA_PIN GPIO_NUM_22
#define LUX_CSL_PIN GPIO_NUM_21
#define DHT_PIN GPIO_NUM_5

FirebaseJson jsonSensorData;

BH1750 lightMeter;
DHT dht;

float prevLuxValue, currentLuxValue;
float prevTempValue, currentTempValue;
float prevHumidityValue, currentHumidityValue;

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

void sensorsSetup() {
    prevLuxValue = 0;
    prevTempValue = 0;
    prevHumidityValue = 0;
    // Inicializacion sensor Luz
    Wire.begin(LUX_SDA_PIN, LUX_CSL_PIN);
    lightMeter.begin(BH1750::ONE_TIME_HIGH_RES_MODE_2);

    // Inicializacion Sensor Humedad y Tempreratura
    pinMode(DHT_PIN, PULLUP);
    dht.setup(DHT_PIN);

    // Set movement sensor timer
    movementTimer = timerTimeout(5000, aulaVacia);

    // Inicializacion Sensor Movimiento
    pinMode(MOVEMENT_PIN, PULLUP);
    attachInterrupt(movement.PIN, movementDetection, HIGH);
}

float readLuxValue() {
    // delay(dht.getMinimumSamplingPeriod());
    currentLuxValue = truncToDec(lightMeter.readLightLevel(),2);
    lightMeter.configure(BH1750::ONE_TIME_HIGH_RES_MODE_2);
    return currentLuxValue;
}

void readSensorsValues() {
    currentLuxValue = readLuxValue();

    if (lapTimer(2500, &tiempoUltimaLecturaDHT)) {
        currentTempValue = truncToDec(dht.getTemperature(),2);
        currentHumidityValue = truncToDec(dht.getHumidity(),2);
    }

}

void uploadSensorsValues() {
    if (!isnan(currentTempValue) && !isnan(currentHumidityValue)) {
        if ((prevTempValue != currentTempValue) || (prevHumidityValue != currentHumidityValue) || (prevLuxValue != currentLuxValue) || movement.roomStateChange) {
            printSensors();

            jsonSensorData.clear();
            jsonSensorData.add("Luminocidad", (double)currentLuxValue);
            jsonSensorData.add("Temperatura", (double)currentTempValue);
            jsonSensorData.add("Humedad", (double)currentHumidityValue);
            jsonSensorData.add("Movimiento", !(double)movement.emptyRoom);
            if (!uploadData(jsonSensorData)) {
                Serial.println("JSON NOT OK");
            }

            prevLuxValue = currentLuxValue;
            prevTempValue = currentTempValue;
            prevHumidityValue = currentHumidityValue;
            movement.roomStateChange = false;
        }
    }
}

void printSensors() {
    Serial.print("\nLuminocidad\tHumedad\t\tTemperatura\tAula Vacia\n");
    Serial.print(currentLuxValue);
    Serial.print(" lux");
    Serial.print("\t");
    Serial.print(currentHumidityValue);
    Serial.print(" %");
    Serial.print("\t \t");
    Serial.print(currentTempValue);
    Serial.print(" C°");
    Serial.print("\t\t");
    Serial.print(movement.emptyRoom);
    Serial.print("\n");
}

void loopSensors() {
    readSensorsValues();
    uploadSensorsValues();
}