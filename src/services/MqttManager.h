#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "services/NetConfig.h"

class MqttManager {
public:
    void begin();
    void tick(bool wifiOK);

    bool isConnected() const { return _connected; }  // TCP + протокол
    bool isAlive() const     { return _alive; }      // self-test прошёл и не протух

    void publishStatus(const String& s);
    void publishPing();

private:
    WiFiClient    _wifiClient;
    PubSubClient  _mqtt;

    bool     _connected    = false;   // client.connected()
    bool     _everReceived = false;   // хоть раз что-то получили
    bool     _alive        = false;   // жив ли по таймауту
    uint32_t _lastRxMs     = 0;       // время последнего RX

    uint32_t _lastAttemptMs = 0;      // последняя попытка connect()
    uint32_t _lastPingMs    = 0;      // последний self-test publish

    static constexpr uint32_t RECONNECT_INTERVAL = 5000;   // мс
    static constexpr uint32_t ALIVE_TIMEOUT      = 60000;  // мс без RX -> dead
    static constexpr uint32_t SELFTEST_INTERVAL  = 45000;  // мс между self-ping

    void reconnect();
    void sendSelfTest();

    static void onMessageStatic(char* topic, byte* payload, unsigned int length);
    void onMessage(char* topic, byte* payload, unsigned int length);

    static MqttManager* instance;
};
