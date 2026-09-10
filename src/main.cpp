#include <Arduino.h>
#include "ChinaNetworkCardController.h"

ChinaNetworkCardController chNCC;

void setup() {
    chNCC.setup();
}

void loop() {
    chNCC.loop();
}