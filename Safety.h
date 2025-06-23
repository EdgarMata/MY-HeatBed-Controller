#ifndef SAFETY_H
#define SAFETY_H

#include <Arduino.h>

// Variáveis globais relacionadas à segurança térmica
extern bool thermalSafetyTriggered;

// Constante para temperatura máxima segura
#define SAFETY_TEMP_MAX 120.0

// Funções relacionadas à segurança térmica
void checkThermalSafety();
void resetThermalSafety();

#endif
