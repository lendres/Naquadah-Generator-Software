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
#include <ShiftRegister74HC595.h>
#include <BatteryMeterShiftRegister.h>
#include <ToggleButton.h>
#include <SoftTimers.h>

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
    // Run this in the loop to update all the lights and controls.
    void update();

  // Light control functions.
  public:
    void greenLightsOn();
    void greenLightsOff();

    void redLightsOn();
    void redLightsOff();

    void whiteLightsOn();
    void whiteLightsOff();

    void blueLightsOff();
    void incrementCurrentBlueLight();
    void rampBlueLightsOn(unsigned int delayBetweenLights);
    void rampBlueLightsOff(unsigned int delayBetweenLights);
    
  private:
    // Initialization functions.
    void initializeInputPins();
    void initializeBatteryMeter();

    void reset(bool resetBlueLights);

    GENERATOR::STATE getGeneratorState();

    void setGeneratorState(GENERATOR::STATE state);
   
    
  private:
    // Arduino pin and control settings.
    Configuration*                  _configuration;
    
    // Main light output.
    ShiftRegister74HC595            _lightsShiftRegister;

    // Battery meter.
    BatteryMeterShiftRegister       _batteryMeter;

    // Overload virtual toggle button.
    ToggleButton                    _overloadButton;

    GENERATOR::STATE                _generatorState;

    // This is the currently active blue light.  It is used to be able to scroll the lights.
    unsigned int                    _currentBlueLight;
    
    // Variable to hold current delay we are using.  We set it to either standard or overload timing.
    unsigned int                    _blueLightDelay;

    // Timer used to determine when to update blue lights and without blocking code execution with "delay."
    SoftTimer                       _blueLightTimer;
};

#endif
