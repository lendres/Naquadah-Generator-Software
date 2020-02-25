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

/*
  You must have the following libraries to run this software.

  Bounce2 by Thomas Ouellet Fredericks, et al.
    - Can be installed from Arduino IDE Library Manager.
    - https://github.com/thomasfredericks/Bounce2

  SoftTimers by Antoine Beauchamp
    - Be careful to get the right library, there are several timer libraries and even more than one "SoftTimer" library.
    - Can be installed from Arduino IDE Library Manager.
    - https://github.com/end2endzone/SoftTimers

  ShiftRegister74HC595 by Timo Denk
    - Can be installed from Arduino IDE Library Manager.
    - https://shiftregister.simsso.de

  BatteryMeter by Lance A. Endres
    - If you recieved this code as part of an archive (zip) it should have been included.
    - https://github.com/lendres/Arduino-BatteryMeter

  BatteryMeterShiftRegister by Lance A. Endres
    - If you recieved this code as part of an archive (zip) it should have been included.
    - https://github.com/lendres/Arduino-BatteryMeterShiftRegister

  ToggleButton by Lance A. Endres
    - If you recieved this code as part of an archive (zip) it should have been included.
    - https://github.com/lendres/Arduino-ToggleButton
*/


#include "enums.h"
#include "configuration.h"
#include "NaquadahGenerator.h"

#include <ShiftRegister74HC595.h>

Configuration       configuration;
//NaquadahGenerator   naquadahGenerator(&configuration);
NaquadahGenerator*   naquadahGenerator;

// Setup function.
void setup()
{
  if (configuration.DebugLevel > DEBUG::OFF)
  {
    Serial.begin(9600);
    Serial.println("Naquadah Generator debuging on.");
  }

  naquadahGenerator = new NaquadahGenerator(&configuration);

  naquadahGenerator->begin();
}

// Main loop.
void loop()
{
  // Check the state and update as needed.
  naquadahGenerator->update();
}
