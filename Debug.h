#ifndef DEBUG_H
#define DEBUG_H

#include <Arduino.h>

// Variáveis globais relacionadas ao debug
extern bool debugMode;

// Funções relacionadas ao debug
void debugMonitor();
void printActiveSegments();
void printActiveSegmentsPeriodically();

#endif
