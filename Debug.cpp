#include "Debug.h"
#include "Pins.h" // Para acessar os pinos e segmentos
#include "TemperatureControl.h" // Para acessar as funções de temperatura

extern bool debugMode; // Declare as external

void debugMonitor() {
    Serial.println("=== System Monitoring ===");
    for (int i = 0; i < 16; i++) {
        float temp = readTemperature(tempSensors[i]);
        Serial.print("Segment ");
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.print(activeSegments[i] ? "Active" : "Inactive");
        Serial.print(" | Temp: ");
        Serial.print(temp);
        Serial.println("°C");
    }

    for (int i = 0; i < 4; i++) {
        Serial.print("Sec ");
        Serial.print(i + 1);
        Serial.print(" | Setpoint: ");
        Serial.print(targetTemp[i]);
        Serial.println("°C");
    }

    Serial.println("=========================");
}

void printActiveSegments() {
    Serial.print("Active segments: ");
    bool found = false;
    for (int i = 0; i < 16; i++) {
        if (activeSegments[i]) {
            if (found) Serial.print(", ");
            Serial.print(i + 1);
            found = true;
        }
    }
    if (!found) {
        Serial.println("None");
    } else {
        Serial.println();
    }
}

void printActiveSegmentsPeriodically() {
    static unsigned long lastPrintTime = 0;
    if (millis() - lastPrintTime >= 5000) {
        lastPrintTime = millis();
        printActiveSegments();
    }
}

float readTemperature(int sensorPin) {
    // Implementation of readTemperature function
}
