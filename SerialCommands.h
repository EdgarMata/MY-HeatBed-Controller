#ifndef SERIAL_COMMANDS_H
#define SERIAL_COMMANDS_H

#include <Arduino.h>

// Funções relacionadas ao processamento de comandos serial
void processSerialCommands();
void processExternalCommand(String command);
void printHelp();
void printSystemStatus();
void deactivateAllSegments(); // Function declaration
void setupPins(); // Function declaration
void updateTemperaturePWM(int section, int start, int end); // Add this line

extern bool thermalSafetyTriggered;
extern bool debugMode;

#endif
