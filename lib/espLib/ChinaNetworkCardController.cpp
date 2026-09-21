//
// Created by Student on 08.08.2026.
//

#include "ChinaNetworkCardController.h"

WebSocketClient webSocket(false);

void ChinaNetworkCardController::setup() {
    Serial.begin(9600);
}

void ChinaNetworkCardController::loop() {
    listenedSerials();


    if (!webSocket.isConnected() && connectionState == 3) {
        connectionState = 1;
    }
    if (connectionState == 1) {
        if (WiFi.status() != WL_CONNECTED) {
            if (millis() - connectionStartTime > MAX_CONNECTION_TIME) {
                reset();
            }
        } else {
            connectionState = 2;
            connectionStartTime = millis();
            webSocket.connect(globalIp, "/", globalPort);
        }
    } else if (connectionState == 2) {
        if (!webSocket.isConnected()) {
            if (millis() - connectionStartTime > MAX_CONNECTION_TIME) {
                reset();
            }
        } else {
            connectionState = 3;
            JsonDocument doc;
            doc["task"] = CONNECT;

            JsonUtility::jsonToSerial(doc, Serial);
        }
    }

    String serverMessage = getReceivedMessage();
    if (serverMessage.length() > 0) {
        JsonDocument doc;
        doc["task"] = RECEIVE;

        JsonDocument payloadDoc;
        if (deserializeJson(payloadDoc, serverMessage) == DeserializationError::Ok) {
            doc["payload"] = payloadDoc;
        } else {
            doc["payload"] = serverMessage;
        }

        JsonUtility::jsonToSerial(doc, Serial);
    }
}

void ChinaNetworkCardController::reset() {
    if (WiFi.status() == WL_CONNECTED) {
        WiFi.disconnect();
    }
    if (webSocket.isConnected()) {
        webSocket.disconnect();
    }
    connectionState = 0;
    JsonDocument doc;
    doc["task"] = RESET;

    JsonUtility::jsonToSerial(doc, Serial);
}

void ChinaNetworkCardController::listenedSerials() {
    if (Serial.available() > 0) {
        parseJson(Serial.readString());
    }
}

void ChinaNetworkCardController::parseJson(const String &string) {
    JsonDocument json;
    JsonUtility::jsonFromString(string, json);

    if (json["task"]) {
        int taskNumber = json["task"];
        switch (taskNumber) {
            case INIT: {
                JsonDocument doc;
                doc["task"] = INIT;

                JsonUtility::jsonToSerial(doc, Serial);
                break;
            }
            case RESET: {
                reset();
                break;
            }
            case ROBOT_STATUS: {
                JsonDocument doc;
                doc["task"] = ROBOT_STATUS;
                doc["wifi"] = WiFi.status();
                doc["websocket"] = webSocket.isConnected();

                JsonUtility::jsonToSerial(doc, Serial);
                break;
            }
            case CONNECT: {
                connectTo(json["ssid"], json["password"], json["ip"], "/", json["port"]);
                break;
            }
            case SEND: {
                String out;
                if (json["payload"].is<const char*>()) {
                    out = json["payload"].as<String>();
                } else {
                    serializeJson(json["payload"], out);
                }
                sendMessage(out);
                break;
            }
        }
    }
}

void ChinaNetworkCardController::sendMessage(const String &message) {
    webSocket.send(message);
}

String ChinaNetworkCardController::getReceivedMessage() {
    if (String receivedMessage; webSocket.getMessage(receivedMessage)) {
        return receivedMessage;
    }
    return "";
}

void ChinaNetworkCardController::connectTo(const String &ssid, const String &password, const String &ip,
                                           const String &path, const int &port) {
    if (webSocket.isConnected()) return;
    WiFi.begin(ssid, password);

    globalIp = ip;
    globalPort = port;

    connectionState = 1;
    connectionStartTime = millis();
    //webSocket.connect(ip, path, port);
}
