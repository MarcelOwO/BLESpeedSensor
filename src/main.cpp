#include <Arduino.h>
#include <memory>

#include "ble_manager/ble_manager.h"
#include "common/templates.h"

float speed = 0.0f;
unsigned long lastUpdate = 0;

std::unique_ptr<BLEManager> bleManager = nullptr;

//unsure if volatile,atomic or using mutex is better here
volatile int counter = 0;

void setup()
{
    Serial.begin(115200);

    pinMode(15,INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(15), []()
    {
        counter++;
    }, FALLING);

    bleManager = make_unique<BLEManager>();

    Serial.println("Waiting a client connection to notify...");
}

void loop()
{
    bleManager->run();

    bleManager->sendData(speed);

    //not sure if std::chrono is more suitable here
    const unsigned long time_now = millis();

    if (time_now - lastUpdate > 1000)
    {
        const unsigned long time_dif = time_now - lastUpdate;
        speed = static_cast<float>(counter) / (time_dif / 1000.0f);
        lastUpdate = time_now;
        counter = 0;
    }
}
