#ifndef PINS_H
#define PINS_H

#include <Arduino.h>

// Relay pins for the 16-segment heating module
extern const int relayPins[16];

// Temperature sensor pins (A0-A15)
extern const int tempSensors[16];

// PWM output pins for the DueX5 (D5, D6, D7, D8)
extern const int pwmOutPins[4];

// PWM input pins from the DueX5 (temperature setpoints from Duet)
extern const int pwmInPins[4];

// Array to track active segments (true = active, false = inactive)
extern bool activeSegments[16];

// Target temperatures for each section
extern float targetTemp[4];

// Number of segments
#define NUM_SEGMENTS 16

// Function to configure all pins
void setupPins();

// Functions to control segments
void activateSegment(int segment);
void deactivateSegment(int segment);
void activateAllSegments();
void deactivateAllSegments();

#endif
