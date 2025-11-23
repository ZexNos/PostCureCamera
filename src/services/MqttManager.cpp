#include "services/MqttManager.h"

MqttManager* MqttManager::instance = nullptr;

// ----------------------------------------------------------------------------
// INIT
// ----------------------------------------------------------------------------

void MqttManager::begin() {
    instance = this;
    _mqtt.setClient(_wifiClient);
    _mqtt.setServer(MQTT_HOST, MQTT_PORT);
    _mqtt.setCallback(MqttManager::onMessageStatic);

    _connected    = false;
    _everReceived = false;
    _alive        = false;
    _lastRxMs     = 0;
    _lastAttemptMs = millis();
    _lastPingMs    = 0;
}

// ----------------------------------------------------------------------------
// CALLBACK
// ----------------------------------------------------------------------------

void MqttManager::onMessageStatic(char* topic, byte* payload, unsigned int length) {
    if (instance) {
        instance->onMessage(topic, payload, length);
    }
}

void MqttManager::onMessage(char* topic, byte* payload, unsigned int length) {
    (void)payload;
    (void)length;

    _everReceived = true;
    _lastRxMs = millis();

    // Можно добавить разбор команд из HA здесь,
    // но для alive достаточно факта получения.
    // Пример отладки:
    // Serial.print("[MQTT] RX topic=");
    // Serial.println(topic);
}

// ----------------------------------------------------------------------------
// SELF-TEST
// ----------------------------------------------------------------------------

void MqttManager::sendSelfTest() {
    if (!_mqtt.connected()) return;

    // Публикуем в топик, на который сами же подписаны.
    // PubSubClient получит это сообщение обратно через callback.
    _mqtt.publish(MQTT_SELFTEST_TOPIC, "probe", false);
    _lastPingMs = millis();
}

// ----------------------------------------------------------------------------
// RECONNECT
// ----------------------------------------------------------------------------

void MqttManager::reconnect() {
    if (_mqtt.connected()) return;

    if (millis() - _lastAttemptMs < RECONNECT_INTERVAL) return;
    _lastAttemptMs = millis();

    bool ok = false;

    if (MQTT_USER && strlen(MQTT_USER) > 0) {
        ok = _mqtt.connect("PostCureCameraClient", MQTT_USER, MQTT_PASS);
    } else {
        ok = _mqtt.connect("PostCureCameraClient");
    }

    if (ok) {
        _mqtt.subscribe(MQTT_SUB_COMMAND);
        _mqtt.subscribe(MQTT_SELFTEST_TOPIC);

        _everReceived = false;
        _alive        = false;
        _lastRxMs     = 0;
        _lastPingMs   = 0;

        // Статус в HA — online
        publishStatus("online");

        // Сразу запускаем self-test ping
        sendSelfTest();
    }
}

// ----------------------------------------------------------------------------
// TICK
// ----------------------------------------------------------------------------

void MqttManager::tick(bool wifiOK) {

    if (!wifiOK) {
        _connected    = false;
        _alive        = false;
        _everReceived = false;
        return;
    }

    if (!_mqtt.connected()) {
        reconnect();
    }

    _connected = _mqtt.connected();

    if (_connected) {
        _mqtt.loop();

        // периодический self-test publish
        if (millis() - _lastPingMs >= SELFTEST_INTERVAL) {
            sendSelfTest();
        }
    }

    // обновляем alive
    if (_connected && _everReceived) {
        _alive = (millis() - _lastRxMs) < ALIVE_TIMEOUT;
    } else {
        _alive = false;
    }
}

// ----------------------------------------------------------------------------
// PUBLISH HELPERS
// ----------------------------------------------------------------------------

void MqttManager::publishStatus(const String& s) {
    if (_mqtt.connected()) {
        _mqtt.publish(MQTT_PUB_STATUS, s.c_str(), true);
    }
}

void MqttManager::publishPing() {
    if (_mqtt.connected()) {
        _mqtt.publish(MQTT_PUB_PING, "1", false);
    }
}
