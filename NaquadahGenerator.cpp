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
  _lightsShiftRegister(8, _configuration->shiftRegisterDataPin, _configuration->shiftRegisterClockPin, _configuration->shiftRegisterLatchPin),
  _batteryMeter(&_lightsShiftRegister, _configuration->batteryMinReading, _configuration->batteryMaxReading, _configuration->Debug),
  _overloadButton(_configuration->stateInputPins[GENERATOR::OVERLOAD]),
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

// This is the main loop.  We keep it at light as possible by only updating when necessary.
void NaquadahGenerator::update()
{
  GENERATOR::STATE newState = getGeneratorState();
  
  if (newState != _generatorState)
  {
    setGeneratorState(newState);
  }

  switch (_generatorState)
  {
    case GENERATOR::OFF:
      // If the off state, we have to have the battery meter monitor the metering button and updating
      // the blue lights accordningly.  This will also allow it to respond if the power level drops while
      // holding the button down.
      _batteryMeter.update();
      break;

    case GENERATOR::PRIMED0:
    case GENERATOR::PRIMED1:
      break;
     
    case GENERATOR::ON:
    case GENERATOR::OVERLOAD:
      // If the on or overload state we need to be updating the current blue light, but only if we have
      // passed the elapsed time.  This also resets the timer.
      if (_blueLightTimer.hasTimedOut())
      {
        incrementCurrentBlueLight();
      }
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
void NaquadahGenerator::incrementCurrentBlueLight()
{
  // All lights off (start with a clean slate).
  _lightsShiftRegister.setNoUpdate(_currentBlueLight, LIGHT::OFF);

  // Increment the light.
  // If we are  the last light, reset to the first.
  if (_currentBlueLight == LIGHT::BLUE5)
  {
    _currentBlueLight = LIGHT::BLUE1;
  }
  else
  {
    _currentBlueLight++;
  }

  _lightsShiftRegister.set(_currentBlueLight, LIGHT::ON);

  _blueLightTimer.setTimeOutTime(_blueLightDelay);
  _blueLightTimer.reset();
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

void NaquadahGenerator::reset(bool resetBlueLights)
{
  greenLightsOff();
  redLightsOff();
  whiteLightsOff();

  // This is here just to allow us to skip reseting the blue lights in the OFF mode.  The
  // battery meter will handle the lights in that mode.
  if (resetBlueLights)
  {
    blueLightsOff();
  }

  // We always want to start with standard delay.  Overload can only be created by first turning to ON, then
  // pressing the overload button.  We set the current blue light to 5 because we are going to call "increment"
  // to turn them on and increment with update to BLUE1 before turning on the light.
  _blueLightDelay   = _configuration->blueLightStandardDelay;
  _currentBlueLight = LIGHT::BLUE5;
}

GENERATOR::STATE NaquadahGenerator::getGeneratorState()
{
  GENERATOR::STATE generatorState = GENERATOR::OFF;
  
  // Start by finding the base state specified by when one of the Cap position sensors goes active.
  for (int i =  GENERATOR::OFF; i <= GENERATOR::ON; i++)
  {
    if (digitalRead(_configuration->stateInputPins[i]) == LOW)
    {
      // We found the activated sensor, save it and break from the loop.
      generatorState  = (GENERATOR::STATE)i;
      break;
    }
  }

  if (generatorState == GENERATOR::ON)
  {
    // If we are on, we need to check to see if overload is active.
    if (_overloadButton.getState())
    {
      generatorState = GENERATOR::OVERLOAD;
    }
  }

  return generatorState;
}

void NaquadahGenerator::setGeneratorState(GENERATOR::STATE state)
{
  // Update our state.
  _generatorState = state;
  
  switch (_generatorState)
  {
    case GENERATOR::OFF:
      // Blue lights are handled by the battery meter when we are in off mode.
      reset(false);
      break;
      
    case GENERATOR::PRIMED0:
      reset(true);
      greenLightsOn();
      break;
      
    case GENERATOR::PRIMED1:
      reset(true);
      redLightsOn();
      break;
      
    case GENERATOR::ON:
      greenLightsOff();
      redLightsOn();
      whiteLightsOn();
      _blueLightDelay = _configuration->blueLightStandardDelay;
      // This will turn on the first light and start the timer.
      incrementCurrentBlueLight();
      break;
      
    case GENERATOR::OVERLOAD:
      greenLightsOff();
      redLightsOn();
      whiteLightsOn();
      // We change the delay and allow it to take effect once the timer times out.
      _blueLightDelay = _configuration->blueLightOverloadDelay;
      break;
  }
}
