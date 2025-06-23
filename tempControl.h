#ifndef TEMP_CONTROL_H
#define TEMP_CONTROL_H

#include <Arduino.h>

// Temperature Control Variables
extern float targetTemp[4];
extern bool activeSegments[16];

// Temperature Cache
extern float cachedTemperatures[16];
extern unsigned long lastReadTime[16];
extern const unsigned long readInterval;

// Function Prototypes
float readTemperature(int sensorPin);
void updateTemperaturePWM(int section, int startSegment, int endSegment);
void controlHeating(int secIndex, int start, int end);
void checkThermalSafety();
void controlHeatingWithPID(int secIndex, int start, int end);
void printSystemStatus(); // Declare the function here

#endif // TEMP_CONTROL_H
