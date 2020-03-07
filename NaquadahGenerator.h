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
#include <ShiftRegister74HC595.h>
#include <BatteryMeterShiftRegister.h>
#include <CycleButton.h>
#include <SoftTimers.h>
#include "enums.h"
#include "configuration.h"

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
    // Initialization.
    void begin();
    
    // Run this in the loop to update all the lights and controls.
    void update();

    Configuration* getConfiguration();

  // Light control functions.
  public:
    void readyIndicatorLightOn();
    void readyIndicatorLightOff();

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

    void rampUpAllLights();
    void rampDownAllLights();

    void startupSequence();
    void setSpecialMode(GENERATOR::SPECIALMODE specialMode);
    
  private:
    // Initialization functions.
    void initializePins();
    void initializeBatteryMeter();

    void reset(bool resetBlueLights);
    void resetControls();
    void resetLights(bool resetBlueLights);

    GENERATOR::STATE getGeneratorState();

    void setGeneratorState(GENERATOR::STATE state);

    void activateChargerKey();

    void debugPrint(const char message[], DEBUG::DEBUGLEVEL level);
    void debugPrint(int message, DEBUG::DEBUGLEVEL level);
    
    void debugPrintLn(const char message[], DEBUG::DEBUGLEVEL level);
    void debugPrintLn(int message, DEBUG::DEBUGLEVEL level);
   
    
  private:
    // Arduino pin and control settings.
    Configuration*                  _configuration;
    
    // Main light output.
    ShiftRegister74HC595<1>         _lightsShiftRegister;

    // Battery meter.
    BatteryMeterShiftRegister<1>    _batteryMeter;

    // Virtual cycle button for special modes.
    CycleButton                     _modeButton;

    // The current state of the generator.  This is the activation arm position.
    GENERATOR::STATE                _generatorState;
    GENERATOR::SPECIALMODE          _specialMode;

    // This is the currently active blue light.  It is used to be able to scroll the lights.
    unsigned int                    _currentBlueLight;
    
    // Variable to hold current delay we are using.  This specifies how often the lights are changed in
    // modes where you have blinking, scrolling, et cetera lights.
    unsigned int                    _lightDelay;

    // Timer used to determine when to update blue lights and without blocking code execution with "delay."
    SoftTimer                       _lightTimer;

    // Timer used to keep charger/booster active, if required.  Some boards shut off if the power draw
    // is low.  This is used to keep them on in the GENERATOR::OFF state where power use is low.
    SoftTimer                       _chargerKeyTimer;
    bool                            _chargerKeyActive;
};

#endif
