#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "xxx";
const char* pass = "xxx";
const char* brokerUser = "vitormelchioretto8@gmail.com";
const char* brokerPass = "vitor3447!$";
const char* broker = "mqtt.dioty.co";

WiFiClient espClient;
PubSubClient client(espClient);

//vitor3447!$ vitormelchioretto8@gmail.com

void setupWifi(){
  delay(100);
  Serial.println("\nConectando em ");

  WiFi.begin(ssid,pass);

  while(WiFi.status() != WL_CONNECTED){
    delay(100);
    Serial.print("-");
  }
  Serial.print("\nConnected to");
  Serial.println(ssid);
}

void reconnect(){
  while(!client.connected()){
    Serial.print("\nConnecting to");
    Serial.println(broker);
    if(client.connect("Melchioretto", brokerUser, brokerPass)){
      Serial.print("\nConnected to");
      Serial.println(broker);
    }else{
      Serial.println("\nTentando conectar dnv ");
      delay(5000);
    }
  }
}

int deviceCount = 0;
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    deviceCount++;
    Serial.print("Endereço: ");
    Serial.println(advertisedDevice.getAddress().toString().c_str());
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
  setupWifi();
  client.setServer(broker, 1883);
  // Inicializa o BLE
  BLEDevice::init("ESP32");

  // Cria um servidor BLE
  pServer = BLEDevice::createServer();

  // Configura os serviços
  BLEService *pService = pServer->createService(BLEUUID((uint16_t)0x1800)); // Serviço genérico de acesso GATT

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
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  // Reinicia a contagem para a próxima digitalização
  deviceCount = 0;

  delay(12000);
}
