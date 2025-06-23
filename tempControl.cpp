#include "tempControl.h"
#include "MY-HeatBed_Controller.h"
#include "SerialCommands.h" // To access global variables

// Declare variables that were removed from MY-HeatBed_Controller.ino
float targetTemp[4] = {0, 0, 0, 0};
bool activeSegments[16] = {false}; // Initially, all segments are inactive

float cachedTemperatures[16] = {0};
unsigned long lastReadTime[16] = {0};
const unsigned long readInterval = 1000;

#include <Arduino.h>
#include <avr/pgmspace.h>
#include "tempControl.h"

// Temperature lookup table (ADC to temperature mapping)
const int tempTable[][2] PROGMEM = {
    {1, 300},    // ADC very low (~0°C)
    {200, 250},  // ADC = 200 corresponds to ~250°C
    {300, 200},  // Example: ADC 300 → 200°C
    {400, 150},
    {500, 120},
    {600, 90},
    {700, 60},
    {800, 30},
    {900, 10},
    {1023, 0}
};

// Define the size of the tempTable array
const int tempTableSize = sizeof(tempTable) / sizeof(tempTable[0]);

float readTemperature(int sensorPin) {
    int sensorIndex = -1;

    // Determine the sensor index based on the pin
    for (int i = 0; i < 16; i++) {
        if (tempSensors[i] == sensorPin) {
            sensorIndex = i;
            break;
        }
    }

    // If the index is not found, return error
    if (sensorIndex == -1) return -999.0;

    // Check if the minimum interval has passed
    if (millis() - lastReadTime[sensorIndex] < readInterval) {
        return cachedTemperatures[sensorIndex]; // Return cached value
    }

    // Update the last read timestamp
    lastReadTime[sensorIndex] = millis();

    // Perform analog reading
    int analogValue = analogRead(sensorPin);

    // Protection against out-of-range readings
    if (analogValue <= 0 || analogValue >= 1023) {
        cachedTemperatures[sensorIndex] = -999.0; // Update cache with error
        return -999.0;
    }

    // Find the closest value in the table
    int i = 0;
    while (pgm_read_word(&tempTable[i][0]) < analogValue && i < tempTableSize - 1) {
        i++;
    }

    // Linear interpolation between two points in the table
    float adcHigh = pgm_read_word(&tempTable[i][0]);
    float tempHigh = pgm_read_word(&tempTable[i][1]);
    float adcLow = pgm_read_word(&tempTable[i - 1][0]);
    float tempLow = pgm_read_word(&tempTable[i - 1][1]);

    float temperature = tempLow + (analogValue - adcLow) * (tempHigh - tempLow) / (adcHigh - adcLow);

    // Update cache with new reading
    cachedTemperatures[sensorIndex] = temperature;

    return temperature;
}

float readTargetTemperature(int pwmPin) {
    int pwmValue = pulseIn(pwmPin, HIGH, PWM_TIMEOUT); // Wait for timeout

    // Validate PWM signal
    if (pwmValue < pwmMinValue || pwmValue > pwmMaxValue) {
        Serial.print("Error: PWM signal out of valid range (");
        Serial.print(pwmValue);
        Serial.println(").");
        return -999.0; // Return error
    }

    // Map PWM value to target temperature
    return map(pwmValue, pwmMinValue, pwmMaxValue, tempMin, tempMax);
}

void updateTemperaturePWM(int secIndex, int start, int end) {
    float sumActive = 0;
    int countActive = 0;
    float sumAll = 0;
    int countAll = 0;

    // Iterate through all segments in the section
    for (int i = start; i <= end; i++) {
        float temp = readTemperature(tempSensors[i]);

        // Sum all valid temperatures
        if (temp != -999.0) {
            sumAll += temp;
            countAll++;
        }

        // Sum only active and valid segments
        if (activeSegments[i] && temp != -999.0) {
            sumActive += temp;
            countActive++;
        }
    }

    float avgTemp;

    // Average of active segments, if any
    if (countActive > 0) {
        avgTemp = sumActive / countActive;
        Serial.print("Sec ");
        Serial.print(secIndex + 1);
        Serial.print(" (active): ");
        Serial.print(avgTemp);
        Serial.println("°C");
    }
    // Average of all sensors if none active
    else if (countAll > 0) {
        avgTemp = sumAll / countAll;
        Serial.print("Sec ");
        Serial.print(secIndex + 1);
        Serial.print(" (none active): ");
        Serial.print(avgTemp);
        Serial.println("°C");
    }
    // No valid sensor found, send default safe value (25°C)
    else {
        avgTemp = 25.0;  // Default safe value
        Serial.print("Sec ");
        Serial.print(secIndex + 1);
        Serial.println(": No valid sensor found. Sending default value (25°C).");
    }

    // 🔥 Corrected: inverting the PWM scale to match Duet's expectation
    int pwmValue = map(avgTemp, tempMin, tempMax, pwmMaxValue, pwmMinValue);
    pwmValue = constrain(pwmValue, pwmMinValue, pwmMaxValue);

    // Send correctly inverted PWM value to DueX5
    analogWrite(pwmOutPins[secIndex], pwmValue);

    Serial.print("Sec ");
    Serial.print(secIndex + 1);
    Serial.print(" Avg Temp Sent: ");
    Serial.print(avgTemp);
    Serial.print("°C -> PWM: ");
    Serial.println(pwmValue);    
}

void controlHeating(int secIndex, int start, int end) {
    float sum = 0;
    int count = 0;

    for (int i = start; i <= end; i++) {
        if (activeSegments[i]) {
            sum += readTemperature(tempSensors[i]);
            count++;
        }
    }

    float avgTemp = (count > 0) ? (sum / count) : 0;
    float target = targetTemp[secIndex];

    // Simple ON/OFF control with hysteresis
    bool heatingOn = (avgTemp < target - TEMP_HYSTERESIS);
    bool heatingOff = (avgTemp > target + TEMP_HYSTERESIS);

    for (int i = start; i <= end; i++) {
        if (activeSegments[i]) {
            if (heatingOn) {
                digitalWrite(relayPins[i], LOW); // Turn on segment
            } else if (heatingOff) {
                digitalWrite(relayPins[i], HIGH); // Turn off segment
            }
        }
    }

    Serial.print("Sec ");
    Serial.print(secIndex + 1);
    Serial.print(" Current Temp: ");
    Serial.print(avgTemp);
    Serial.print("°C | Setpoint: ");
    Serial.print(target);
    Serial.println("°C");
}

void checkThermalSafety() {
    for (int i = 0; i < 16; i++) {
        float temp = readTemperature(tempSensors[i]);
        if (temp > MAX_SAFE_TEMPERATURE) {
            thermalSafetyTriggered = true;
            deactivateAllSegments(); // Deactivate all segments
            Serial.print("ALERT: Critical temperature detected in segment ");
            Serial.print(i + 1);
            Serial.print(" (");
            Serial.print(temp);
            Serial.println("°C). All segments deactivated!");
            break;
        }
    }
}

float calculatePID(int segmentIndex, float currentTemp, float targetTemp) {
    unsigned long now = millis();
    float deltaTime = (now - pidLastUpdate[segmentIndex]) / 1000.0; // Time in seconds
    pidLastUpdate[segmentIndex] = now;

    // Calculate error
    float error = targetTemp - currentTemp;

    // Calculate proportional term
    float proportional = pidKp * error;

    // Calculate integral term
    pidIntegral[segmentIndex] += error * deltaTime;
    float integral = pidKi * pidIntegral[segmentIndex];

    // Calculate derivative term
    float derivative = pidKd * (error - pidLastError[segmentIndex]) / deltaTime;
    pidLastError[segmentIndex] = error;

    // Sum terms to get PID output
    float output = proportional + integral + derivative;

    // Limit output between 0 and 1 (for relay control)
    return constrain(output, 0.0, 1.0);
}

void controlHeatingWithPID(int secIndex, int start, int end) {
    for (int i = start; i <= end; i++) {
        if (activeSegments[i]) {
            float currentTemp = readTemperature(tempSensors[i]);
            float target = targetTemp[secIndex];

            // Calculate PID output
            float pidOutput = calculatePID(i, currentTemp, target);

            // Turn relay on or off based on PID output
            if (pidOutput > PID_OUTPUT_THRESHOLD) {
                digitalWrite(relayPins[i], LOW); // Turn on segment
            } else {
                digitalWrite(relayPins[i], HIGH); // Turn off segment
            }

            // Print information to Serial
            Serial.print("Segment ");
            Serial.print(i + 1);
            Serial.print(" | Current Temp: ");
            Serial.print(currentTemp);
            Serial.print("°C | Setpoint: ");
            Serial.print(target);
            Serial.print("°C | PID Output: ");
            Serial.println(pidOutput);
        }
    }
}

void printSystemStatus() {
    Serial.println("=== System Status ===");
    Serial.println("Debug Mode: " + String(debugMode ? "Enabled" : "Disabled"));
    Serial.println("Thermal Safety State: " + String(thermalSafetyTriggered ? "Triggered" : "Normal"));
    for (int i = 0; i < 16; i++) {
        Serial.print("Segment ");
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.print(activeSegments[i] ? "Active" : "Inactive");
        Serial.print(" | Temp: ");
        Serial.print(readTemperature(tempSensors[i])); // Ensure readTemperature and tempSensors are declared
        Serial.println("°C");
    }
    for (int i = 0; i < 4; i++) {
        Serial.print("Sec ");
        Serial.print(i + 1);
        Serial.print(" | Setpoint: ");
        Serial.print(targetTemp[i]);
        Serial.println("°C");
    }
    Serial.println("=====================");
}
