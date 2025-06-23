#ifndef TEMPERATURE_CONTROL_H
#define TEMPERATURE_CONTROL_H

#include <Arduino.h>

// Cache para armazenar leituras de temperatura
extern float cachedTemperatures[16];

// Funções relacionadas ao controle de temperatura
void setupPins();
float readTemperature(int sensorPin);
float calculatePID(int segmentIndex, float currentTemp, float targetTemp);
void controlHeatingWithPID(int secIndex, int start, int end);
void updateTemperaturePWM(int secIndex, int start, int end);
void configurePWMRange(int minPWM, int maxPWM, float minTemp, float maxTemp);

#endif
