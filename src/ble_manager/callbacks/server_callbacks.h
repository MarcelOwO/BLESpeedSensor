//
// Created by marce on 6/12/2025.
//

#ifndef SERVER_CALLBACKS_H
#define SERVER_CALLBACKS_H
#include "NimBLEServer.h"

class ServerCallbacks final : public NimBLEServerCallbacks
{
public:
    void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override;

    void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override;

    bool connected() const;

private:
    bool deviceConnected = false;
};
#endif //SERVER_CALLBACKS_H
