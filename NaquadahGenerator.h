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

#ifndef NAQUADAHGENERATOR_H
#define NAQUADAHGENERATOR_H

#include <Arduino.h>
#include "enums.h"
#include "configuration.h"
#include <ShiftRegister.h>
#include <BatteryMeterShiftRegister.h>

class NaquadahGenerator
{
  private:
  
  // Constructors.
  public:
    // Default contstructor.
    NaquadahGenerator(Configuration* configuration);

    // Default destructor.
    ~NaquadahGenerator();

  // Public interface.
  public:
    void setState(GENERATOR::STATE state);

  // Light control functions.
  public:
    void greenLightsOn();
    void greenLightsOff();

    void redLightsOn();
    void redLightsOff();

    void whiteLightsOn();
    void whiteLightsOff();

    void blueLightsOff();
    void incrementCurrentBlueLight(GENERATOR::STATE currentState);
    void rampBlueLightsOn(unsigned int delayBetweenLights);
    void rampBlueLightsOff(unsigned int delayBetweenLights);
    
  private:
    // Initialization functions.
    void initializeInputPins();
    void initializeBatteryMeter();

    void reset();
   
    
  private:
    // Arduino pin and control settings.
    Configuration*                   _configuration;
    
    // Main light output.
    ShiftRegister                   _lightsShiftRegister;

    // Battery meter.
    BatteryMeterShiftRegister       _batteryMeter;

    GENERATOR::STATE                _generatorState;

    // This is the currently active blue light.  It is used to be able to scroll the lights.
    unsigned int                    _currentBlueLight;
    
    // Variable to hold current delay we are using.  We set it to either standard or overload timing.
    unsigned int                    _blueLightDelay;
};

#endif
