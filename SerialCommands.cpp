#include "SerialCommands.h"
#include "Pins.h" // Para acessar NUM_SEGMENTS e funções de controle de segmentos
#include "Debug.h"
#include "Safety.h"
#include "TemperatureControl.h"

// Define the external variables
bool debugMode = false;
bool thermalSafetyTriggered = false;

void processSerialCommands() {
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n');
        command.trim();

        Serial.print("Received command: \"");
        Serial.print(command);
        Serial.println("\"");

        if (command == "DEBUG ON") {
            debugMode = true;
            Serial.println("Debug mode enabled.");
        } else if (command == "DEBUG OFF") {
            debugMode = false;
            Serial.println("Debug mode disabled.");
        } else if (command == "STATUS") {
            printSystemStatus();
        } else if (command.startsWith("SET_PWM_RANGE")) {
            int minPWM = command.substring(14, command.indexOf(" ", 14)).toInt();
            int maxPWM = command.substring(command.indexOf(" ", 14) + 1, command.lastIndexOf(" ")).toInt();
            float minTemp = command.substring(command.lastIndexOf(" ") + 1, command.lastIndexOf(" ", command.lastIndexOf(" ") - 1)).toFloat();
            float maxTemp = command.substring(command.lastIndexOf(" ") + 1).toFloat();

            configurePWMRange(minPWM, maxPWM, minTemp, maxTemp);
        } else if (command == "ON ALL") {
            if (!thermalSafetyTriggered) {
                activateAllSegments();
                Serial.println("All segments activated.");
            } else {
                Serial.println("Error: System in thermal safety state. Reset before continuing.");
            }
        } else if (command == "OFF ALL") {
            deactivateAllSegments();
            Serial.println("All segments deactivated.");
        } else if (command.startsWith("ON")) {
            int segmentNumber = command.substring(3).toInt();
            if (segmentNumber >= 1 && segmentNumber <= 16) {
                if (!thermalSafetyTriggered) {
                    activateSegment(segmentNumber);
                    Serial.print("Segment ");
                    Serial.print(segmentNumber);
                    Serial.println(" activated.");
                } else {
                    Serial.println("Error: System in thermal safety state. Reset before continuing.");
                }
            } else {
                Serial.println("Error: Invalid segment number. Use HELP to see commands.");
            }
        } else if (command.startsWith("OFF")) {
            int segmentNumber = command.substring(4).toInt();
            if (segmentNumber >= 1 && segmentNumber <= 16) {
                deactivateSegment(segmentNumber);
                Serial.print("Segment ");
                Serial.print(segmentNumber);
                Serial.println(" deactivated.");
            } else {
                Serial.println("Error: Invalid segment number. Use HELP to see commands.");
            }
        } else if (command == "HELP") {
            printHelp();
        } else if (command == "RESET_SAFETY") {
            resetThermalSafety();
        } else {
            Serial.println("Error: Unrecognized command. Use HELP to see commands.");
        }
    }
}

void processExternalCommand(String command) {
    command.trim();

    if (command == "ON ALL") {
        if (!thermalSafetyTriggered) {
            activateAllSegments();
            Serial.println("All segments activated (Duet).");
        } else {
            Serial.println("Error: Thermal safety active (Duet).");
        }
    } else if (command == "OFF ALL") {
        deactivateAllSegments();
        Serial.println("All segments deactivated (Duet).");
    } else if (command.startsWith("ON")) {
        int segmentNumber = command.substring(3).toInt();
        if (segmentNumber >= 1 && segmentNumber <= 16) {
            if (!thermalSafetyTriggered) {
                activateSegment(segmentNumber);
                Serial.print("Segment ");
                Serial.print(segmentNumber);
                Serial.println(" activated (Duet).");
            } else {
                Serial.println("Error: Thermal safety active (Duet).");
            }
        } else {
            Serial.println("Error: Invalid segment number (Duet).");
        }
    } else if (command.startsWith("OFF")) {
        int segmentNumber = command.substring(4).toInt();
        if (segmentNumber >= 1 && segmentNumber <= 16) {
            deactivateSegment(segmentNumber);
            Serial.print("Segment ");
            Serial.print(segmentNumber);
            Serial.println(" deactivated (Duet).");
        } else {
            Serial.println("Error: Invalid segment number (Duet).");
        }
    } else if (command == "RESET_SAFETY") {
        resetThermalSafety();
        Serial.println("Thermal safety state reset (Duet).");
    } else if (command == "DEBUG ON") {
        debugMode = true;
        Serial.println("Debug mode enabled (Duet).");
    } else if (command == "DEBUG OFF") {
        debugMode = false;
        Serial.println("Debug mode disabled (Duet).");
    } else if (command == "HELP") {
        printHelp();
    } else if (command == "STATUS") {
        printSystemStatus();
    } else {
        Serial.print("Error: Unrecognized command (Duet): ");
        Serial.println(command);
    }
}

void printHelp() {
    Serial.println("Available commands:");
    Serial.println("  ON ALL              - Activate all segments");
    Serial.println("  OFF ALL             - Deactivate all segments");
    Serial.println("  ON <n>              - Activate segment <n> (1-16)");
    Serial.println("  OFF <n>             - Deactivate segment <n> (1-16)");
    Serial.println("  SET_PWM_RANGE <minPWM> <maxPWM> <minTemp> <maxTemp> - Configure PWM range");
    Serial.println("  DEBUG ON            - Enable debug mode");
    Serial.println("  DEBUG OFF           - Disable debug mode");
    Serial.println("  STATUS              - Display system status");
    Serial.println("  HELP                - Display this list of commands");
    Serial.println("  RESET_SAFETY        - Reset thermal safety state");
}

void printSystemStatus() {
    Serial.println("System Status:");
    // Add more details about the system status here
    // For example:
    // Serial.println("Temperature: 25°C");
    // Serial.println("Heater: ON");
    // Serial.println("Fan: OFF");
}

void configurePWMRange(int minPWM, int maxPWM, float minTemp, float maxTemp) {
    // Implementation of configurePWMRange function
}

void activateAllSegments() {
    // Implementation of activateAllSegments function
}

void deactivateAllSegments() {
    for (int i = 0; i < NUM_SEGMENTS; i++) {
        digitalWrite(relayPins[i], LOW);
        activeSegments[i] = false;
    }
}

void activateSegment(int segmentNumber) {
    // Implementation of activateSegment function
}

void deactivateSegment(int segmentNumber) {
    // Implementation of deactivateSegment function
}
