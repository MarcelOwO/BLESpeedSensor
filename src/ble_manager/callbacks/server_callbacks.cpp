//
// Created by marce on 6/12/2025.
//

#include "server_callbacks.h"

#include "NimBLEDevice.h"

void ServerCallbacks::onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo)
{
    deviceConnected = true;
}

void ServerCallbacks::onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason)
{
    NimBLEDevice::whiteListAdd(connInfo.getAddress());
    deviceConnected = false;
}

bool ServerCallbacks::connected() const
{
    return deviceConnected;
}


