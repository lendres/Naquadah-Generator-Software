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
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

struct Configuration
{
  const int Debug                     = 1;
  const int numberOfStatePins         = 5;
  const int* stateInputPins           = new int[5] {3, 4, 5, 6, 2};
  
  const int switchRegisterDataPin   = 16;
  const int switchRegisterClockPin  = 14;
  const int switchRegisterLatchPin  = 15;
  
  // Blue top lights.
  const int numberOfBlueLights      = 5;
  
   // Green indicator light to indicate Arduino is ready.
  const int readyIndicatorPin       = 12;
  
  // Battery meter.
  unsigned int batteryMeterPin      = 1;
  unsigned int batterySensePin      = A3;
  // Set the min and max reading values that correspond to 2.7 and 4.2 volts (for a lithium battery).
  unsigned int batteryMinReading    = 552;
  unsigned int batteryMaxReading    = 865;
  
  // Values for timing.
  const unsigned int blueLightStandardDelay  = 120;
  const unsigned int blueLightOverloadDelay  = (int)(blueLightStandardDelay / 2.0);
  const unsigned int startUpDelay            = 1.5*blueLightStandardDelay;
};

#endif
