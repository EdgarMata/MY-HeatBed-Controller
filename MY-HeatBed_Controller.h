#ifndef MY_HEATBED_CONTROLLER_H
#define MY_HEATBED_CONTROLLER_H

#include <Arduino.h>

// Pin Definitions
extern const int relayPins[16];
extern const int tempSensors[16];
extern const int pwmOutPins[4];
extern const int pwmInPins[4];

// Temperature Control Variables
extern float targetTemp[4];
extern bool activeSegments[16];

// Thermistor Configuration
#define PULLUP_RESISTOR 10000.0 // 10kΩ
#define A 0.001129148
#define B 0.000234125
#define C 0.0000000876741

extern const int tempTable[][2] PROGMEM;

// Constants and Macros
#define NUM_SEGMENTS 16
#define NUM_SECTIONS 4
#define TEMP_HYSTERESIS 2.0
#define PWM_TIMEOUT 25000
#define DEBUG_INTERVAL 5000
#define SAFETY_TEMP_MAX 120.0
#define PID_OUTPUT_THRESHOLD 0.5
#define MAX_SAFE_TEMPERATURE 120.0

// PWM Configuration Variables
extern int pwmMinValue;
extern int pwmMaxValue;
extern float tempMin;
extern float tempMax;

// Temperature Cache
extern float cachedTemperatures[16];
extern unsigned long lastReadTime[16];
extern const unsigned long readInterval;

// PID Control Variables
extern float pidKp;
extern float pidKi;
extern float pidKd;
extern float pidIntegral[16];
extern float pidLastError[16];
extern unsigned long pidLastUpdate[16];

// Debug Mode
extern bool debugMode;

// Thermal Safety
extern bool thermalSafetyTriggered;

// Function Prototypes
float readTemperature(int sensorPin);
void configurePWMRange(int minPWM, int maxPWM, float minTemp, float maxTemp);
void setupPins();
void updateAllSections();
void printActiveSegmentsPeriodically();
void checkThermalSafety();
void resetThermalSafety();
float calculatePID(int segmentIndex, float currentTemp, float targetTemp);
void controlHeatingWithPID(int secIndex, int start, int end);
void debugMonitor();
void updateTemperaturePWM(int secIndex, int start, int end);
void printActiveSegments();
void processSerialCommands();
void processExternalCommand(String command);

#endif // MY_HEATBED_CONTROLLER_H
