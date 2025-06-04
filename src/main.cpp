#include <Arduino.h>
#include <NimBLEDevice.h>

NimBLECharacteristic* pCharacteristic = nullptr;

bool deviceConnected = false;
bool oldDeviceConnected = false;

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

class ServerCallbacks final : public NimBLEServerCallbacks {
    void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override {
        deviceConnected = true;
    };

    void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override {
        NimBLEDevice::whiteListAdd(connInfo.getAddress());
        deviceConnected = false;
    }
} serverCallbacks;

void onAdvComplete(NimBLEAdvertising* pAdvertising) {
    Serial.println("Advertising stopped");
    if (deviceConnected) {
        return;
    }
    pAdvertising->setScanFilter(false, false);
    pAdvertising->start();
}

volatile int counter = 0;
float speed = 0.0f;
unsigned long lastUpdate = 0;

void setup() {
    Serial.begin(115200);

    pinMode(15,INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(15), []() {
        counter++;
    }, FALLING);

    NimBLEDevice::init("SpeedSensorBLE");
    NimBLEDevice::setDeviceName("SpeedSensorBLE");

    NimBLEServer* pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(&serverCallbacks);
    pServer->advertiseOnDisconnect(false);

    NimBLEService* pService = pServer->createService(SERVICE_UUID);

    pCharacteristic =
    pService->createCharacteristic(CHARACTERISTIC_UUID,
                                   NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE |
                                   NIMBLE_PROPERTY::NOTIFY);
    pService->start();

    NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setName("SpeedSensorBLE");
    pAdvertising->enableScanResponse(false);
    pAdvertising->setAdvertisingCompleteCallback(onAdvComplete);
    pAdvertising->start();
    Serial.println("Waiting a client connection to notify...");
}

void loop() {
    if (deviceConnected) {
        std::array<uint8_t, sizeof(float)> speedBytes{};
        Serial.println(speed);
        memcpy(speedBytes.data(), &speed, sizeof(float));
        pCharacteristic->setValue(speedBytes.data(), sizeof(float));
        pCharacteristic->notify();
    }

    if (!deviceConnected && oldDeviceConnected) {
        NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
        if (NimBLEDevice::getWhiteListCount() > 0) {
            pAdvertising->setScanFilter(false, true);
        }
        pAdvertising->start(30 * 1000);
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }

    if (deviceConnected && !oldDeviceConnected) {
        oldDeviceConnected = deviceConnected;
    }

    const unsigned long time_now = millis();

    if (time_now - lastUpdate > 1000) {
        const unsigned long time_dif = time_now - lastUpdate;
        speed = static_cast<float>(counter) / (time_dif / 1000.0f);
        lastUpdate = time_now;
        counter = 0;
    }
}
