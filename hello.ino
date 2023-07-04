#include <BLEDevice.h>
#include <BLEServer.h>
#include <WiFi.h>
#include <PubSubClient.h>
#define LED_NUMERO 13

const char* ssid = "";
const char* pass = "";
const char* broker = "test.mosquitto.org";

WiFiClient espClient;
PubSubClient client(espClient);

void setupWifi() {
    delay(5000);
    Serial.println("\nConectando em ");

    WiFi.begin(ssid, pass);

    while (WiFi.status() != WL_CONNECTED) {
        delay(5000);
        Serial.print("-");
    }

    Serial.print("\nConnected to ");
    Serial.println(ssid);
}

void reconnect() {
    while (!client.connected()) {
        Serial.print("\nConnecting to");
        Serial.println(broker);

        if (client.connect("Melchioretto")) {
            Serial.print("\nConnected to");
            Serial.println(broker);
        }
        else {
            Serial.println("\nTentando conectar dnv ");
            delay(5000);
        }
    }
}

struct BLEDeviceInfo {
    String name;
    String address;
    int rssi;
    float distance;
};

std::vector<BLEDeviceInfo> devices;

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        int rssi = advertisedDevice.getRSSI();
        float distance = calculateDistance(rssi);

        if (distance <= 10.0) {
            BLEDeviceInfo device;
            device.name = advertisedDevice.getName().c_str();
            device.address = advertisedDevice.getAddress().toString().c_str();
            device.rssi = rssi;
            device.distance = distance;

            devices.push_back(device);

            Serial.print("Nome: ");
            Serial.println(device.name);
            Serial.print("Endereço: ");
            Serial.println(device.address);
            Serial.print("RSSI: ");
            Serial.print(device.rssi);
            Serial.print(" dBm, Distância: ");
            Serial.print(device.distance);
            Serial.println(" metros");
            Serial.println("--------------------");
        }
    }

float calculateDistance(int rssi) {
    int txPower = -70; // Podemos modificar
    float ratio = rssi * 1.0 / txPower;
    if (ratio < 1.0) {
        return pow(ratio, 10);
    }
    else {
        float distance = (0.89976) * pow(ratio, 7.7095) + 0.111;
        return distance;
    }
}
};

BLEServer *pServer = NULL;
BLEScan* pBLEScan = NULL;

void setup() {
    Serial.begin(115200);
    delay(5000);
    setupWifi();
    client.setServer(broker, 1883);
    pinMode(LED_NUMERO, OUTPUT);
    // Inicializa o BLE
    BLEDevice::init("ESP32");
    // Cria um servidor BLE
    pServer = BLEDevice::createServer();
    // Configura os serviços
    BLEService *pService = pServer->createService(BLEUUID((uint16_t)0x1800));
    // Cria um caractere de nome do dispositivo
    BLECharacteristic *pCharacteristic = pService->createCharacteristic(
        BLEUUID((uint16_t)0x2A00), // UUID do Characteristic de nome do dispositivo
        BLECharacteristic::PROPERTY_READ);
    pCharacteristic->setValue("ESP32"); // Define o nome do dispositivo
    // Adiciona o característica ao serviço
    pService->addCharacteristic(pCharacteristic);
    // Inicia o serviço
    pService->start();
    // Inicia a publicação do anúncio BLE
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(pService->getUUID());
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06); // taxas rápidas de publicidade
    pAdvertising->setMinPreferred(0x12);
    Serial.println("ESP32 iniciado e descobrível como 'ESP32'");
    // Configura a digitalização e a contagem de dispositivos
    pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true);
}

void loop() {
    // Inicia a digitalização
    BLEScanResults foundDevices = pBLEScan->start(5);

    // Exibe a contagem de dispositivos encontrados
    if (devices.size() > 0) {
        Serial.print("Número de dispositivos Bluetooth encontrados: ");
        Serial.println(devices.size());
    }

    // Envia os detalhes dos dispositivos encontrados para o tópico MQTT
    if (!client.connected()) {
        reconnect();
    }

    for (const auto& device : devices) {
        String message =  "Endereço: " + device.address + ", Distância: " + String(device.distance) + " metros";
        client.publish("melchioretto", message.c_str(), 30);

        digitalWrite(LED_NUMERO, HIGH); // Liga o LED
        delay(1000); // Aguarda 1 segundo
        digitalWrite(LED_NUMERO, LOW); // Desliga o LED
        delay(1000); // Aguarda 1 segundo antes de piscar para o próximo dispositivo
    }
    delay(100000);

    client.loop();
    devices.clear();
}
