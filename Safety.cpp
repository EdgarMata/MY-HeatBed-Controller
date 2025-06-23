#include "Safety.h"
#include "Pins.h" // Para acessar as funções deactivateAllSegments
#include "TemperatureControl.h" // Para acessar as funções de temperatura

extern bool thermalSafetyTriggered; // Declare as external

void checkThermalSafety() {
    for (int i = 0; i < 16; i++) {
        float temp = readTemperature(tempSensors[i]);
        if (temp > SAFETY_TEMP_MAX) {
            thermalSafetyTriggered = true;
            deactivateAllSegments(); // Desativa todos os segmentos
            Serial.print("ALERT: Critical temperature detected in segment ");
            Serial.print(i + 1);
            Serial.print(" (");
            Serial.print(temp);
            Serial.println("°C). All segments deactivated!");
            break;
        }
    }
}

void resetThermalSafety() {
    thermalSafetyTriggered = false;
    Serial.println("Thermal safety state reset. System ready for use.");
}
