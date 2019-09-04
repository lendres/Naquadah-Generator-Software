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
//
#include "enums.h"

#include "configuration.h"

#include "NaquadahGenerator.h"

//#include <ShiftRegister.h>
//#include <BatteryMeterShiftRegister.h>


Configuration configuration;

NaquadahGenerator naquadahGenerator(&configuration);

// GET RID OF
int currentBlueLight              = 0;
//ShiftRegister lightsShiftRegister(8, switchRegisterDataPin, switchRegisterClockPin, switchRegisterLatchPin);
//BatteryMeterShiftRegister batteryMeter(lightsShiftRegister, 552, 865, DEBUG);

// Setup function.
void setup()
{
  if (configuration.Debug)
  {
    Serial.begin(9600);
  }
  

  // Make sure we start with all lights off.
//  lightsShiftRegister.setAllLow();

  // Run startup sequence.  A light display just for the fun of it.
  startupSequence(&naquadahGenerator);

  // All ready, turn on "ready" indicator light.
  digitalWrite(configuration.readyIndicatorPin, HIGH);
}

// Main loop.
void loop()
{
  // The logic for determining which state we are in is separated from the logic for behavior.  This keeps
  // things a lot cleaner.  There is a small cost associated with this, but nothing significant.
  GENERATOR::STATE currentState = getGeneratorState(&configuration);

  // Set the red, green, and white lights.
  naquadahGenerator.setState(currentState);



  // If we are off, we will check to see if we are to display the battery level.   We don't need to do anything,
  // the battery meter will do it.  We just need an additional check to make sure we are in the OFF state.  We
  // don't want the battery check button to work otherwise.
  if (currentState == GENERATOR::OFF)
  {
    Serial.println("Battery check");
//    batteryMeter.runBatteryMeter();
  }

  // Pause by the length of delay between blue light scrolling.  This could be improved by scanning inputs as fast
  // as possible and only incrementing the blue lights once a timer has elapsed.
//  delay(blueLightDelay);
}
