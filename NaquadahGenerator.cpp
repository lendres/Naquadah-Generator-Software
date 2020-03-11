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
//#include "LightSequences.h"

NaquadahGenerator::NaquadahGenerator(Configuration* configuration) :
	_configuration(configuration),
	_lightsShiftRegister(_configuration->shiftRegisterDataPin, _configuration->shiftRegisterClockPin, _configuration->shiftRegisterLatchPin),
	_batteryMeter(&_lightsShiftRegister, _configuration->batteryMinReading, _configuration->batteryMaxReading, Battery::LEVEL5),
	_batteryMeterButton(_configuration->batteryMeterActivationPin),
	_modeButton(_configuration->modeButtonPin, GENERATOR::SPECIALMODE05),
	_generatorState(GENERATOR::STATE::OFF),
	_currentBlueLight(0),
	_lightDelay(_configuration->blueLightStandardDelay),
	_chargerKeyActive(false)
{
	initializePins();
	initializeBatteryMeter();
}

NaquadahGenerator::~NaquadahGenerator()
{
}

void NaquadahGenerator::begin()
{
	digitalWrite(_configuration->chargerKeyPin, HIGH);
	
	// Setup charger activation button timer.
	_chargerKeyTimer.setTimeOutTime(_configuration->chargerKeyDelay);
	_chargerKeyTimer.reset();


	// Run startup sequence.  A light display just for the fun of it.
	if (_configuration->runStartUpSequence)
	{
		startupSequence();
	}


	// Initial state.  Just to make sure.
	setGeneratorState(GENERATOR::OFF);

	// All ready, turn on "ready" indicator light.
	readyIndicatorLightOn();

	debugPrintLn("Generator state initialized.", DEBUG::STANDARD);
}

// This is the main loop.  We keep it at light as possible by only updating when necessary.
void NaquadahGenerator::update()
{
	// Check the current state.
	GENERATOR::STATE newState = getGeneratorState();

	// If the current state is different than the set one, we update everything.  Otherwise, we
	// don't update to save time.
	if (newState != _generatorState)
	{
		setGeneratorState(newState);
	}

	// The setGeneratorState function will configure everything when the state changes.  Now we have to handle
	// the events that need to be updated every loop.
	switch (_generatorState)
	{
		case GENERATOR::OFF:
		{
			GENERATOR::SPECIALMODE modeButtonValue = (GENERATOR::SPECIALMODE)_modeButton.getValue();

			if (modeButtonValue != _modeButtonValue)
			{
				setSpecialMode(modeButtonValue);
			}

			void runSpecialMode();

			break;
		}

		case GENERATOR::PRIMED0:
		case GENERATOR::PRIMED1:
		{
			break;
		}
		
		case GENERATOR::ON:
		{
			// Look to see if the mode button has been used to change the blue light timing.  This is how we implement "overload" timing of
			// the lights.  The more you press the button, the faster the lights go, until the maximum value is hit.  After which, the light
			// timing will reset (because the value return by getValue resets to zero).
			//
			// The value is updated with every loop to make sure we capture a button push.  The user won't see an effect until the timer times
			// out and the lights update with the new timing value.
			_modeButtonValue = (GENERATOR::SPECIALMODE)_modeButton.getValue();

			// If in the on or overload state we need to be updating the current blue light, but only if we have
			// passed the elapsed time.  The timer gets reset as part of the increment function.
			if (_lightTimer.hasTimedOut())
			{
				_lightDelay = _configuration->blueLightStandardDelay - _modeButtonValue*_configuration->blueLightOverloadIncrement;
				incrementCurrentBlueLight();
			}
			break;
		}
	}

	// If we need to update the charger key (button) to keep the power on.
	if (_chargerKeyTimer.hasTimedOut())
	{
		activateChargerKey();
	}
}

Configuration* NaquadahGenerator::getConfiguration()
{
	return _configuration;
}

void NaquadahGenerator::readyIndicatorLightOn()
{
	digitalWrite(_configuration->readyIndicatorPin, HIGH);
}

void NaquadahGenerator::readyIndicatorLightOff()
{
	digitalWrite(_configuration->readyIndicatorPin, LOW);
}

void NaquadahGenerator::greenLightsOn()
{
	debugPrintLn("Green lights on.", DEBUG::STANDARD);
	debugPrint("Pin: ", DEBUG::STANDARD);
	debugPrintLn(LIGHT::GREEN, DEBUG::STANDARD);
	debugPrint("Value: ", DEBUG::STANDARD);
	debugPrintLn(LIGHT::ON, DEBUG::STANDARD);

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
		// First set the state for each light.  We don't need to update every
		// time through the loop, so we use the no update version.
		_lightsShiftRegister.setNoUpdate(i, LIGHT::OFF);
	}

	// The states have been set, so call update now to do the update all at once.
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

	_lightTimer.setTimeOutTime(_lightDelay);
	_lightTimer.reset();
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

void NaquadahGenerator::rampUpAllLights()
{
	// Forwards on the blue lights.
	rampBlueLightsOn(_configuration->startUpDelay);

	delay(2*_configuration->startUpDelay);
	greenLightsOn();
	redLightsOn();

	delay(2*_configuration->startUpDelay);
	whiteLightsOn();
}

void NaquadahGenerator::rampDownAllLights()
{
	whiteLightsOff();

	delay(2*_configuration->startUpDelay);
	greenLightsOff();
	redLightsOff();

	// Backwards on the blue lights.
	delay(_configuration->startUpDelay);
	rampBlueLightsOff(_configuration->startUpDelay);
}

// Do a cool startup.
void NaquadahGenerator::startupSequence()
{
	rampUpAllLights();

	// Pause with all the lights on.
	delay(12*_configuration->startUpDelay);

	rampDownAllLights();
}

// Inputs for hall effect sensors and overload button.
void NaquadahGenerator::initializePins()
{
	// Initialize ready light input pin.
	pinMode(_configuration->readyIndicatorPin, OUTPUT);
	readyIndicatorLightOff();
		
	// Set all the state pins as input.
	for (int i = 0; i < _configuration->numberOfStatePins; i++)
	{
		// Set as input (read from them).
		pinMode(_configuration->stateInputPins[i], INPUT);
		
		// Use internal resistor to pull pin to high.  They are pulled low to indicate activation.
		digitalWrite(_configuration->stateInputPins[i], HIGH);
	}

	// Pin used to keep charger/booster active.
	if (_configuration->useChargerKey)
	{
		pinMode(_configuration->chargerKeyPin, OUTPUT);
	}
}

void NaquadahGenerator::initializeBatteryMeter()
{
	// This is the pin that is connected to the battery positive termina.
	_batteryMeter.setSensingPin(_configuration->batteryMeterSensePin);

	// These are the pins that the lights are connected to.
	unsigned int switchRegisterBlueLightPins[] = {LIGHT::BLUE1, LIGHT::BLUE2, LIGHT::BLUE3, LIGHT::BLUE4, LIGHT::BLUE5};
	_batteryMeter.setLightPins(switchRegisterBlueLightPins, HIGH);

	// This is the pin used to indicate when the battery meter should run.
	_batteryMeter.setActivationButton(_batteryMeterButton);

	// Tell the battery meter to initialize.
	_batteryMeter.begin();
}

void NaquadahGenerator::reset()
{
	resetLights();
	resetControls();
}

void NaquadahGenerator::resetControls()
{
	// We always want to start with standard delay.  Overload can only be created by first turning to ON, then
	// pressing the overload button.  We set the current blue light to 5 because we are going to call "increment"
	// to turn them on and increment with update to BLUE1 before turning on the light.
	_lightDelay       = _configuration->blueLightStandardDelay;
	_currentBlueLight = LIGHT::BLUE5;

	// Reset the toggle buttons so the initial state is active (off).
	_modeButton.reset();
	_modeButtonValue = GENERATOR::SPECIALMODEOFF;  
}

void NaquadahGenerator::resetLights()
{
	greenLightsOff();
	redLightsOff();
	whiteLightsOff();
	blueLightsOff();
	readyIndicatorLightOn();
}

GENERATOR::STATE NaquadahGenerator::getGeneratorState()
{
	// This function reads the state sensing pins and determines what the current state is.  This function must NOT set
	// the value of the member variable (_generatorState).  That gets done in the setGeneratorState function.  Separating
	// out the two lets us determine if we actually need to change the state (if it stays the same, we do nothing).

	// Default to current state.  We only change when a new sensor or button is pressed.  If a sensor is found to be
	// in the on state, this will get over written with the new state value.  However, if the control arm is in the process
	// of moving to another position, no pins will read as on, therefore, we keep the current value.
	GENERATOR::STATE generatorState = _generatorState;
	
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

	debugPrint("Generator state: ", DEBUG::VERBOSE);
	debugPrintLn(generatorState, DEBUG::VERBOSE);

	return generatorState;
}

void NaquadahGenerator::setGeneratorState(GENERATOR::STATE state)
{
	// Update our state.
	_generatorState = state;
	
	// For the case of switching between PRIMED1 AND ON, we don't want to turn off the red lights then turn
	// them back on.  Doing so might cause a flicker.  Therefore, we don't call reset when switching between
	// those two.
	switch (_generatorState)
	{
		case GENERATOR::OFF:
			reset();
			break;
			
		case GENERATOR::PRIMED0:
			reset();
			greenLightsOn();
			break;
			
		case GENERATOR::PRIMED1:
			greenLightsOff();
			redLightsOn();
			whiteLightsOff();
			blueLightsOff();

			resetControls();
			break;
			
		case GENERATOR::ON:
			greenLightsOff();
			redLightsOn();
			whiteLightsOn();

			resetControls();

			// This will turn on the first light and start the timer.
			incrementCurrentBlueLight();
			break;
	}
}

void NaquadahGenerator::setSpecialMode(GENERATOR::SPECIALMODE specialMode)
{
	_modeButtonValue = specialMode;
	reset();

	switch (_modeButtonValue)
	{
		case GENERATOR::SPECIALMODEOFF:
			break;

		case GENERATOR::SPECIALMODE01:
			readyIndicatorLightOff();
			rampUpAllLights();
			readyIndicatorLightOn();
			break;

		case GENERATOR::SPECIALMODE02:
			greenLightsOn();
			redLightsOn();
			break;

		case GENERATOR::SPECIALMODE03:
		case GENERATOR::SPECIALMODE04:
		case GENERATOR::SPECIALMODE05:
			break;
	}
}

void NaquadahGenerator::runSpecialMode()
{
	switch (_modeButtonValue)
	{
		case GENERATOR::SPECIALMODEOFF:
			// In the off state and with the special modes off, we have to have the battery meter monitor update.
			// This checkes the metering button and updates the blue lights accordingly.
			_batteryMeter.update();
			break;

		case GENERATOR::SPECIALMODE01:
			break;

		case GENERATOR::SPECIALMODE02:
			reset();
			greenLightsOn();
			redLightsOn();
			break;

		case GENERATOR::SPECIALMODE03:
		case GENERATOR::SPECIALMODE04:
		case GENERATOR::SPECIALMODE05:
			break;
	}
}

void NaquadahGenerator::activateChargerKey()
{
	if (_configuration->useChargerKey)
	{
		// Debug message.
		debugPrint("Checking charger key button: ", DEBUG::STANDARD);
		
		if (_chargerKeyActive)
		{
			debugPrintLn("Disconnect key button.", DEBUG::STANDARD);
		 
			// Disconnect the key button.
			digitalWrite(_configuration->chargerKeyPin, LOW);
			_chargerKeyTimer.setTimeOutTime(_configuration->chargerKeyDelay);
		}
		else
		{
			debugPrintLn("Connect key button.", DEBUG::STANDARD);

			// Connect the key button.
			digitalWrite(_configuration->chargerKeyPin, HIGH);
			_chargerKeyTimer.setTimeOutTime(_configuration->chargerKeyActivationTime);
		}
	}

	_chargerKeyTimer.reset();
	_chargerKeyActive = !_chargerKeyActive;
}

void NaquadahGenerator::debugPrint(const char message[], DEBUG::DEBUGLEVEL level)
{
	if (_configuration->DebugLevel >= level)
	{
		Serial.print(message);
	}
}

void NaquadahGenerator::debugPrint(int message, DEBUG::DEBUGLEVEL level)
{
	if (_configuration->DebugLevel >= level)
	{
		Serial.print(message);
	}
}

void NaquadahGenerator::debugPrintLn(const char message[], DEBUG::DEBUGLEVEL level)
{
	if (_configuration->DebugLevel >= level)
	{
		Serial.println(message);
	}
}

void NaquadahGenerator::debugPrintLn(int message, DEBUG::DEBUGLEVEL level)
{
	if (_configuration->DebugLevel >= level)
	{
		Serial.println(message);
	}
}