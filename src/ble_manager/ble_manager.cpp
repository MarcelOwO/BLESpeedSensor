//
// Created by marce on 6/12/2025.
//

#include "ble_manager.h"
#include "common/templates.h"


BLEManager::BLEManager() = default;

BLEManager::~BLEManager() = default;

void BLEManager::init()
{
    CHECK(NimBLEDevice::init("SpeedSensorBLE"), "NimBLEDevice init failed");

    CHECK(NimBLEDevice::setDeviceName("SpeedSensorBLE"), "NimBLE setDeviceName failed");

    const auto pServer = NimBLEDevice::createServer();

    CHECK((pServer != nullptr), "NimbleServer is null");

    server_callback = make_unique<ServerCallbacks>();

    CHECK((server_callback != nullptr), "ServerCallbacks is null");

    pServer->setCallbacks(server_callback.get());

    pServer->advertiseOnDisconnect(false);

    const auto pService = pServer->createService(SERVICE_UUID);
    CHECK(pService != nullptr, "NimbleService is null");

    pCharacteristic =
        pService->createCharacteristic(CHARACTERISTIC_UUID, READ | WRITE | NOTIFY);

    CHECK(pCharacteristic != nullptr, "NimBLECharacteristic is null");

    CHECK(pService->start(), "Failed to start service");

    pAdvertising = NimBLEDevice::getAdvertising();
    CHECK(pAdvertising->addServiceUUID(SERVICE_UUID), "Failed to add service UUID");

    CHECK(pAdvertising->setName("SpeedSensorBLE"), "Failed to set advertising name");

    pAdvertising->enableScanResponse(true);

    pAdvertising->setAdvertisingCompleteCallback([this](NimBLEAdvertising* pAdvertising)
    {
        Serial.println("Advertising stopped");
        if (server_callback->connected())
        {
            return;
        }
        pAdvertising->setScanFilter(false, false);
        pAdvertising->start();
    });

    CHECK(pAdvertising->start(), "Failed to start advertising");
}

void BLEManager::run()
{
    const auto deviceConnected = server_callback->connected();

    if (!deviceConnected && oldDeviceConnected)
    {
        if (NimBLEDevice::getWhiteListCount() > 0)
        {
            pAdvertising->setScanFilter(false, true);
        }
        pAdvertising->start(30 * 1000);
        oldDeviceConnected = deviceConnected;
    }

    if (deviceConnected && !oldDeviceConnected)
    {
        oldDeviceConnected = deviceConnected;
    }
}

void BLEManager::sendData(const float speed)
{
    if (!server_callback->connected())
    {
        return;
    }

    speedBytes.fill(0);
    memcpy(speedBytes.data(), &speed, sizeof(float));
    pCharacteristic->setValue(speedBytes.data(), sizeof(float));
}
