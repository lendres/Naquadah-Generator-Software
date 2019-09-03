/*
  Copyright (c) 2019 Lance A. Endres

  This program is free software: you can redistribute it and/or modify
  it under the terms of the Attribution-NonCommercial 4.0 International
  (CC BY-NC 4.0) license as published by the Creative Commons Corporation
  or (at your option) any later version.

  You may not use this software for commercial works or profit from it.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  https://creativecommons.org/licenses/by-nc/4.0/legalcode
*/

#include "enums.h"
#include "pins.h"
#include "BatteryMeter.h"

#define DEBUG 1


// This is the currently active blue light.  It is used to be able to scroll the lights.
int currentBlueLight              = 0;

// Values for timing.
const int blueLightStandardDelay  = 120;
const int blueLightOverloadDelay  = (int)(blueLightStandardDelay / 2.0);
const int startUpDelay            = 1.5*blueLightStandardDelay;

// Variable to hold current delay we are using.  We set it to either standard or overload timing.
int blueLightDelay                = blueLightStandardDelay;

// Battery meter.
BatteryMeter batteryMeter(DEBUG);

// Setup function.
void setup()
{
  if (DEBUG)
  {
    Serial.begin(9600);
  }
  
  // Initialize pins for lights and input.
  initializeStaticLightPins();
  initializeBlueLightPins();
  initializeInputPins();

  // Set the min and max reading values that correspond to 2.7 and 4.2 volts (for a lithium battery).
  // Set the values and run the battery meter setup (begin).
  batteryMeter.setMinMaxReadingValues(552, 865);
  batteryMeter.setSensingPin(batterySensePin);
  batteryMeter.setLightPins(blueLightPins, BatteryMeter::LEVEL5, HIGH);
  batteryMeter.setActivationPin(BatteryMeter::MOMENTARY, batteryPin, LOW);
  batteryMeter.begin();

  // Run startup sequence.  A light display just for the fun of it.
  startupSequence();

  // All ready, turn on "ready" indicator light.
  digitalWrite(readyIndicatorPin, HIGH);
}

// Main loop.
void loop()
{
  // The logic for determining which state we are in is separated from the logic for behavior.  This keeps
  // things a lot cleaner.  There is a small cost associated with this, but nothing significant.
  GENERATORSTATE currentState = getGeneratorState();

  // Set the red, green, and white lights.
  setStaticLights(currentState);

  // Handle blue light incrementing (scrolling), if applicable, or turn them all off and reset.
  setBlueLightDelay(currentState);
  incrementCurrentBlueLight(currentState);

  // If we are off, we will check to see if we are to display the battery level.   We don't need to do anything,
  // the battery meter will do it.  We just need an additional check to make sure we are in the OFF state.  We
  // don't want the battery check button to work otherwise.
  if (currentState == OFF)
  {
    Serial.println("Battery check");
    batteryMeter.runBatteryMeter();
  }

  // Pause by the length of delay between blue light scrolling.  This could be improved by scanning inputs as fast
  // as possible and only incrementing the blue lights once a timer has elapsed.
  delay(blueLightDelay);
}
