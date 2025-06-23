#include <Arduino.h>
#include "MY-HeatBed_Controller.h"
#include "setupPins.h"

void setupPins() {
    // Configure temperature sensor pins as input
    for (int i = 0; i < 16; i++) {
        pinMode(tempSensors[i], INPUT);
    }

    // Configure PWM output pins as output
    for (int i = 0; i < 4; i++) {
        pinMode(pwmOutPins[i], OUTPUT);
    }

    // Configure PWM input pins as input
    for (int i = 0; i < 4; i++) {
        pinMode(pwmInPins[i], INPUT);
    }

    // Configure relay pins as output
    for (int i = 0; i < 16; i++) {
        pinMode(relayPins[i], OUTPUT);
        digitalWrite(relayPins[i], HIGH); // Keep relays off (active LOW)
    }
}
