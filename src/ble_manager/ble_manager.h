//
// Created by marce on 6/12/2025.
//

#ifndef BLEMANAGER_H
#define BLEMANAGER_H

#include <memory>
#include <NimBLEDevice.h>

#include "callbacks/server_callbacks.h"

class BLEManager
{
public:
    BLEManager();
    ~BLEManager();

    BLEManager(BLEManager&) = delete;
    BLEManager& operator=(const BLEManager&) = delete;
    BLEManager(BLEManager&&) = delete;
    BLEManager& operator=(BLEManager&&) = delete;

    void init();

    void run();

    void sendData(float speed);

private:
    NimBLECharacteristic* pCharacteristic = nullptr;
    NimBLEAdvertising* pAdvertising = nullptr;

    std::unique_ptr<ServerCallbacks> server_callback = nullptr;

    const std::string SERVICE_UUID{"4fafc201-1fb5-459e-8fcc-c5c9c331914b"};
    const std::string CHARACTERISTIC_UUID{"beb5483e-36e1-4688-b7f5-ea07361b26a8"};

    bool oldDeviceConnected = false;

    std::array<uint8_t, sizeof(float)> speedBytes{};
};


#endif //BLEMANAGER_H
