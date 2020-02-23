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
  // SETUP CONFIGURATION SETTINGS.
  // If true (1) debugging messages are printed.
  const DEBUG::DEBUGLEVEL   DebugLevel                   = DEBUG::STANDARD;

  // Just set the input pins the 4 hall effect sensors and overload push button is
  // on.  The number of state pins should not be changed.  If an overload button is
  // not used, just use a pull up resistor to keep that pin high and it will be ignored.
  const int                 numberOfStatePins           = 5;
  const int*                stateInputPins              = new int[7] {3, 4, 5, 6, 2, 8, 9};

  const int                 shiftRegisterDataPin        = 14;
  const int                 shiftRegisterClockPin       = 15;
  const int                 shiftRegisterLatchPin       = 16;
  
   // Green indicator light to indicate Arduino is ready.
  const int                 readyIndicatorPin           = 12;

  // Some chargers/boosters power down if you don't draw power from them.  Some have a
  // button used to power them on.  This is used to activate the buttom.
  const bool                useChargerKey               = true;
  const int                 chargerKeyPin               = 10;
  const unsigned int        chargerKeyDelay             = 10000;
  const unsigned int        chargerKeyActivationTime    = 500;
  
  // Battery meter.
  unsigned int              batteryMeterActivationPin   = 7;
  unsigned int              batteryMeterSensePin        = A3;
  // Set the min and max reading values that correspond to 2.7 and 4.2 volts (for a lithium battery).
  unsigned int              batteryMinReading           = 646;
  unsigned int              batteryMaxReading           = 865;

  // BEHAVIOR SETTINGS.
  // Values for timing.
  const unsigned int        blueLightStandardDelay      = 130;
  const unsigned int        blueLightOverloadDelay      = (int)(blueLightStandardDelay / 2.20);
  const unsigned int        startUpDelay                = 1.5*blueLightStandardDelay;

  // Start up sequence.
  const bool                runStartUpSequence          = false;
};

#endif
