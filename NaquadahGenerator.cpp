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

#include "NaquadahGenerator.h"

NaquadahGenerator::NaquadahGenerator(Configuration* configuration) :
  _configuration(configuration),
  _lightsShiftRegister(8, _configuration->switchRegisterDataPin, _configuration->switchRegisterClockPin, _configuration->switchRegisterLatchPin),
  _batteryMeter(_lightsShiftRegister, _configuration->batteryMinReading, _configuration->batteryMaxReading, _configuration->Debug),
  _generatorState(GENERATOR::STATE::OFF),
  _currentBlueLight(0),
  _blueLightDelay(_configuration->blueLightStandardDelay)
{
  initializeInputPins();
  initializeBatteryMeter();
}

NaquadahGenerator::~NaquadahGenerator()
{
}

// Sets the state of the top red and green cap lights and the arm and rail white lights.
void NaquadahGenerator::setState(GENERATOR::STATE state)
{ 
  switch (state)
  {
    case GENERATOR::OFF:
      reset();
      break;
      
    case GENERATOR::PRIMED0:
      reset();
      greenLightsOn();
      break;
      
    case GENERATOR::PRIMED1:
      reset();
      redLightsOn();
      break;
      
    case GENERATOR::ON:
      greenLightsOff();
      redLightsOn();
      whiteLightsOn();
      _blueLightDelay = _configuration->blueLightStandardDelay;
      break;
      
    case GENERATOR::OVERLOAD:
      greenLightsOff();
      redLightsOn();
      whiteLightsOn();
      _blueLightDelay = _configuration->blueLightOverloadDelay;
      break;
  }
}

void NaquadahGenerator::greenLightsOn()
{
  _lightsShiftRegister.set(LIGHT::GREEN, LIGHT::ON);
}

void NaquadahGenerator::greenLightsOff()
{
  _lightsShiftRegister.set(LIGHT::GREEN, LIGHT::OFF);
}

void NaquadahGenerator::redLightsOn()
{
  _lightsShiftRegister.set(LIGHT::RED, LIGHT::ON);
}

void NaquadahGenerator::redLightsOff()
{
  _lightsShiftRegister.set(LIGHT::RED, LIGHT::OFF);
}

void NaquadahGenerator::whiteLightsOn()
{
  _lightsShiftRegister.set(LIGHT::WHITE, LIGHT::ON);
}

void NaquadahGenerator::whiteLightsOff()
{
  _lightsShiftRegister.set(LIGHT::WHITE, LIGHT::OFF);
}

void NaquadahGenerator::blueLightsOff()
{
  for (int i = LIGHT::BLUE1; i <= LIGHT::BLUE5; i++)
  {
    _lightsShiftRegister.setNoUpdate(i, LIGHT::OFF);
  }
  _lightsShiftRegister.updateRegisters();
}

// This does the main work of scrolling the blue lights.  If the lights are on, the current
// light is turned off and the next one turned on.  If the blue lights are off, a reset is done.
void NaquadahGenerator::incrementCurrentBlueLight(GENERATOR::STATE currentState)
{
  // All lights off (start with a clean slate).
  _lightsShiftRegister.setNoUpdate(_currentBlueLight, LIGHT::OFF);

  // If we are at the last light, reset to the first.
  if (_currentBlueLight == LIGHT::BLUE5)
  {
    _currentBlueLight = LIGHT::BLUE1;
  }

  _lightsShiftRegister.set(_currentBlueLight++, LIGHT::ON);
}

void NaquadahGenerator::rampBlueLightsOn(unsigned int delayBetweenLights)
{
  // Forwards on the blue lights.
  for (int i = LIGHT::BLUE1; i <= LIGHT::BLUE5; i++)
  {
    delay(delayBetweenLights);
    _lightsShiftRegister.set(i, LIGHT::ON);
  }
}

void NaquadahGenerator::rampBlueLightsOff(unsigned int delayBetweenLights)
{
  for (int i = LIGHT::BLUE5; i >= LIGHT::BLUE1 ; i--)
  {
    delay(delayBetweenLights);
    _lightsShiftRegister.set(i, LIGHT::OFF);
  }
}

// Inputs for hall effect sensors and overload button.
void NaquadahGenerator::initializeInputPins()
{
  // Set all the state pins as input.
  for (int i = 0; i < _configuration->numberOfStatePins; i++)
  {
    // Set as input (read from them).
    pinMode(_configuration->stateInputPins[i], INPUT);
    
    // Use internal resistor to pull pin to high.  They are pulled low to indicate activation.
    digitalWrite(_configuration->stateInputPins[i], HIGH);
  }
}

void NaquadahGenerator::initializeBatteryMeter()
{
  // Set the values and run the battery meter setup (begin).
  _batteryMeter.setSensingPin(_configuration->batterySensePin);
  unsigned int switchRegisterBlueLightPins[] = {LIGHT::BLUE1, LIGHT::BLUE2, LIGHT::BLUE3, LIGHT::BLUE4, LIGHT::BLUE5};
  _batteryMeter.setLightPins(switchRegisterBlueLightPins, BatteryMeter::LEVEL5, HIGH);
  _batteryMeter.setActivationPin(BatteryMeter::MOMENTARY, _configuration->batteryMeterPin, LOW);
  _batteryMeter.begin();
}

void NaquadahGenerator::reset()
{
  greenLightsOff();
  redLightsOff();
  whiteLightsOff();
  blueLightsOff();
  _blueLightDelay = _configuration->blueLightStandardDelay;
  _currentBlueLight = 0;
}
