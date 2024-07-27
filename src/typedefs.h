#include <Arduino.h>

typedef void (*mcb)(byte, byte); // Mode callback
typedef void (*pcb)(byte, int);  // Position callback
typedef void (*scb)(byte, byte); // Scene callback
