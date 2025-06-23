/*
 * HeatBed Controller
 * 
 * Este código controla um sistema de aquecimento de cama quente com 16 segmentos,
 * usando sensores de temperatura e controle PID. Inclui funcionalidades de segurança térmica,
 * modo de debug e comandos Serial para controle e configuração.
 * 
 * Autor: Edgar Mata
 * Empresa: My Machines
 * Site: https://my-machines.net
 * Licença: Creative Commons Attribution 4.0 International
 */

#include <Arduino.h> // Include Arduino header for pin definitions
#include <math.h>    // Library for logarithmic calculations
#include <avr/pgmspace.h> // Required for PROGMEM usage
#include "Pins.h"
#include "TemperatureControl.h"
#include "SerialCommands.h"
#include "Safety.h"
#include "Debug.h"

// ====== Pin Definitions ======
// Relay pins for the 16-segment heating module
const int relayPins[16] = {
    22, 24, 26, 28,  // Segments 1-4
    30, 32, 34, 36,  // Segments 5-8
    38, 40, 42, 44,  // Segments 9-12
    53, 51, 49, 47   // Segments 13-16
};

// Temperature sensor pins (A0-A15)
const int tempSensors[16] = {
    A0, A1, A2, A3,     // Sensors 1-4
    A4, A5, A6, A7,     // Sensors 5-8
    A8, A9, A10, A11,   // Sensors 9-12
    A12, A13, A14, A15  // Sensors 13-16
};

// PWM output pins for the DueX5 (D5, D6, D7, D8)
const int pwmOutPins[4] = {5, 6, 7, 8};

// PWM input pins from the DueX5 (temperature setpoints from Duet)
const int pwmInPins[4] = {9, 10, 11, 12};

// ====== Temperature Control Variables ======
// Target temperatures for each section
float targetTemp[4] = {0, 0, 0, 0};

// Array to track active segments (true = active, false = inactive)
bool activeSegments[16] = {false}; // Initially, all segments are inactive

// ====== Thermistor Configuration ======
// Pull-up resistor value (adjust based on your circuit)
#define PULLUP_RESISTOR 10000.0 // 10kΩ

// Steinhart-Hart coefficients for a 100k NTC thermistor
#define A 0.001129148
#define B 0.000234125
#define C 0.0000000876741

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

// ====== Constants and Macros ======
// Remover a definição de NUM_SEGMENTS daqui
// #define NUM_SEGMENTS 16
#define NUM_SECTIONS 4           // Total number of sections
#define TEMP_HYSTERESIS 2.0      // Temperature hysteresis (in °C)
#define PWM_TIMEOUT 25000        // Timeout for PWM signal reading (in microseconds)
#define DEBUG_INTERVAL 5000      // Interval for debug messages (in ms)
#define SAFETY_TEMP_MAX 120.0    // Maximum safe temperature (in °C)
#define PID_OUTPUT_THRESHOLD 0.5 // Threshold for PID output to activate relays

// ====== PWM Configuration Variables ======
int pwmMinValue = 0;    // Minimum PWM signal value
int pwmMaxValue = 255; // Maximum PWM signal value
float tempMin = 0.0;    // Minimum temperature corresponding to PWM
float tempMax = 280.0;  // Maximum temperature corresponding to PWM

// ====== Temperature Cache ======
// Cache for storing temperature readings
float cachedTemperatures[16] = {0};

// Timestamps for the last temperature readings
unsigned long lastReadTime[16] = {0};

// Minimum interval between temperature readings (in ms)
const unsigned long readInterval = 1000;

// ====== PID Control Variables ======
// PID gains
float pidKp = 2.0; // Proportional gain
float pidKi = 0.5; // Integral gain
float pidKd = 1.0; // Derivative gain

// PID state variables
float pidIntegral[16] = {0}; // Integral term for each segment
float pidLastError[16] = {0}; // Last error for derivative calculation
unsigned long pidLastUpdate[16] = {0}; // Last PID update timestamp

// ====== Debug Mode ======
extern bool debugMode; // Declare as external

// ====== Thermal Safety ======
extern bool thermalSafetyTriggered; // Declare as external

// ====== Function Prototypes ======
// Apenas as funções que ainda estão neste arquivo
void updateAllSections();
void controlHeating(int segment, int start, int end);
void setupPins();
void updateTemperaturePWM(int section, int start, int end);
void controlHeatingWithPID(int section, int start, int end); // Add this line

// ====== Setup Function ======
// Initializes the system, configures pins, and prints a startup message
void setup() {
    Serial.begin(115200); // Initialize Serial communication
    Serial1.begin(115200);  // Comunicação com Duet
    setupPins();          // Configure all pins
    Serial.println("Arduino Mega ready to receive commands from Duet.");
    Serial.println("Temperature control system initialized!");
}

// ====== Main Loop ======
// Handles commands, updates sections, and manages heating control
void loop() {
    if (!thermalSafetyTriggered) {
        processSerialCommands();          // Process incoming Serial commands
        updateAllSections();              // Update temperature and PWM for all sections
        printActiveSegmentsPeriodically(); // Print active segments periodically

        for (int i = 0; i < NUM_SECTIONS; i++) {
            controlHeatingWithPID(i, i * (NUM_SEGMENTS / NUM_SECTIONS), (i * (NUM_SEGMENTS / NUM_SECTIONS)) + (NUM_SEGMENTS / NUM_SECTIONS) - 1);
        }

        checkThermalSafety(); // Check for thermal safety violations

        if (debugMode) {
            debugMonitor();
        }
    } else {
        Serial.println("System in thermal safety state. Use RESET_SAFETY command to reset.");
        delay(DEBUG_INTERVAL); // Prevent message spamming
    }
    delay(DEBUG_INTERVAL); // General delay to avoid overloading the system

    if (Serial1.available()) {
        String recebido = Serial1.readStringUntil('\n');
        recebido.trim();  // Remove extra characters
        Serial.print("Recebido da Duet: ");
        Serial.println(recebido);
        processExternalCommand(recebido); // Process the received command
    }
}

// ====== Function to update all sections ======
void updateAllSections() {
    for (int i = 0; i < 4; i++) {
        updateTemperaturePWM(i, i * 4, (i * 4) + 3);
        controlHeating(i, i * 4, (i * 4) + 3);
    }
}

// ====== Function to control heating ======
void controlHeating(int segment, int start, int end) {
    // Implementation of controlHeating function
}

// ====== Function to setup pins ======
void setupPins() {
    // Define the pin setup logic here
    // For example:
    pinMode(LED_BUILTIN, OUTPUT);
    // Add other pin setups as needed
}

// ====== Function to update temperature PWM ======
void updateTemperaturePWM(int section, int start, int end) {
    // Define the logic for updating temperature PWM here
    // For example:
    for (int i = start; i <= end; i++) {
        // Read temperature from sensor
        float temperature = analogRead(tempSensors[i]);
        // Convert ADC value to temperature
        // Apply PID control or other logic to update PWM
        // Example: analogWrite(pwmOutPins[section], pwmValue);
    }
}

// ====== Function to control heating with PID ======
void controlHeatingWithPID(int section, int start, int end) {
    for (int i = start; i <= end; i++) {
        // Read the current temperature
        float currentTemp = analogRead(tempSensors[i]);

        // Calculate the error
        float error = targetTemp[section] - currentTemp;

        // Calculate the integral term
        pidIntegral[i] += error;

        // Calculate the derivative term
        float derivative = error - pidLastError[i];

        // Calculate the PID output
        float output = (pidKp * error) + (pidKi * pidIntegral[i]) + (pidKd * derivative);

        // Update the last error
        pidLastError[i] = error;

        // Update the last PID update timestamp
        pidLastUpdate[i] = millis();

        // Control the relay based on the PID output
        if (output > PID_OUTPUT_THRESHOLD) {
            digitalWrite(relayPins[i], HIGH);
            activeSegments[i] = true;
        } else {
            digitalWrite(relayPins[i], LOW);
            activeSegments[i] = false;
        }
    }
}