#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

int deviceCount = 0;

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    deviceCount++;
    Serial.print("Nome: ");
    Serial.println(advertisedDevice.getName().c_str());
    Serial.print("Endereço: ");
    Serial.println(advertisedDevice.getAddress().toString().c_str());
    Serial.print("RSSI: ");
    Serial.print(advertisedDevice.getRSSI());
    Serial.print(" dBm, Distância: ");
    Serial.print(calculateDistance(advertisedDevice.getRSSI()));
    Serial.println(" metros");
    Serial.println("--------------------");
  }
  
  float calculateDistance(int rssi) {
    int txPower = -59; // RSSI na distância de 1 metro
    float ratio = rssi * 1.0 / txPower;
    if (ratio < 1.0) {
      return pow(ratio, 10);
    } else {
      float distance = (0.89976) * pow(ratio, 7.7095) + 0.111;
      return distance;
    }
  }
};

BLEServer *pServer = NULL;
bool deviceConnected = false;

void setup() {
  Serial.begin(115200);

  // Inicializa o BLE
  BLEDevice::init("Virus");

  // Cria um servidor BLE
  pServer = BLEDevice::createServer();

  // Configura os serviços
  BLEService *pService = pServer->createService(BLEUUID((uint16_t)0x1800)); // Serviço genérico de acesso GATT

  // Cria um caractere de nome do dispositivo
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
      BLEUUID((uint16_t)0x2A00), // UUID do Characteristic de nome do dispositivo
      BLECharacteristic::PROPERTY_READ);
  pCharacteristic->setValue("Virus"); // Define o nome do dispositivo como "Virus"

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

  Serial.println("ESP32 iniciado e descobrível como 'Virus'");
}

void loop() {
  // Configura a digitalização e a contagem de dispositivos
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);

  // Inicia a digitalização
  BLEScanResults foundDevices = pBLEScan->start(5);

  // Exibe a contagem de dispositivos encontrados
  if (deviceCount > 0) {
    Serial.print("Número de dispositivos Bluetooth encontrados: ");
    Serial.println(deviceCount);
  }

  // Reinicia a contagem para a próxima digitalização
  deviceCount = 0;

  delay(12000);
}
