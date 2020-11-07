#include "iloc.h"

//aux functions

int registerCounter = 0;

int labelCounter = 0;

int getRegister() {
  registerCounter++;
  return registerCounter;
}

int getLabel() {
  labelCounter++;
  return labelCounter;
}