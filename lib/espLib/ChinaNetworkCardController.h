//
// Created by Student on 08.08.2026.
//

#ifndef ESPCHINAWIFINUMBER1_CHINANETWORKCARDCONTROLLER_H
#define ESPCHINAWIFINUMBER1_CHINANETWORKCARDCONTROLLER_H

#include "ESP8266WiFi.h"
#include "WebSocketClient.h"
#include "Tasks.h"
#include "JsonUtility.h"
#include <Arduino.h>

class ChinaNetworkCardController
{

uint8_t connectionState;
static constexpr int MAX_CONNECTION_TIME = 10000;
unsigned long connectionStartTime;
String globalIp;
uint16_t globalPort;

public:
    ChinaNetworkCardController() = default;
    void setup();
    void loop();

    void reset();
    void listenedSerials();
    void parseJson(const String& string);

    static void sendMessage(const String& message);
    static String getReceivedMessage();
    void connectTo(const String& ssid, const String& password, const String& ip, const String& path, const int& port);
};


#endif //ESPCHINAWIFINUMBER1_CHINANETWORKCARDCONTROLLER_H
