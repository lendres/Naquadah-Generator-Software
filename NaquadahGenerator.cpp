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
	_shiftRegister(_configuration->shiftRegisterDataPin, _configuration->shiftRegisterClockPin, _configuration->shiftRegisterLatchPin),
	_batteryMeter(&_shiftRegister, _configuration->batteryMinReading, _configuration->batteryMaxReading, Battery::LEVEL5),
	_batteryMeterButton(_configuration->batteryMeterActivationPin),
	_modeButton(_configuration->modeButtonPin, GENERATOR::NUMBEROFSPECIALMODES-1),
	_generatorState(GENERATOR::STATE::OFF),
	_currentBlueLight(0),
	_lightDelay(_configuration->blueLightStandardDelay),
	_audioSerial(_configuration->rxFromAudioTxPin, _configuration->txToAudioRxPin),
	_vsUart(&_audioSerial, _configuration->audioResetPin)
{
}

NaquadahGenerator::~NaquadahGenerator()
{
}

void NaquadahGenerator::begin()
{
	// Initialize ready light input pin.
	pinMode(_configuration->readyIndicatorPin, OUTPUT);

	// We are going to do some work, so make sure the "ready" indicator light is off.
	readyIndicatorLightOff();

	_shiftRegister.set(AUDIO::UG, HIGH);
	_shiftRegister.set(AUDIO::RESET, HIGH);
	_shiftRegister.set(AUDIO::STATECHANGE, HIGH);
	_shiftRegister.set(AUDIO::ON, HIGH);

	// Audio set up.
	// Set up the levels we want to use.
	// This sets the lowest level to 1 instead of 0.
	_vsUart.useLowerLevelOne(true);

	// Set the maximum level as 5.
	_vsUart.setMaximumLevel(VS1000UART::VOLUME5);

	// Set the minimum volume used.
	_vsUart.setMinimumVolume(100);
	
	// Set the maximum volume used.
	_vsUart.setMaximumVolume(204);

	// Audio start up.
	_audioSerial.begin(9600);
	_vsUart.begin();

	// Battery meter initialization.
	initializeBatteryMeter();
		
	// Set all the state pins as input.
	for (int i = 0; i < GENERATOR::NUMBEROFSTATES; i++)
	{
		// Set as input (read from them).
		pinMode(_configuration->stateInputPins[i], INPUT);
		
		// Use internal resistor to pull pin to high.  They are pulled low to indicate activation.
		digitalWrite(_configuration->stateInputPins[i], HIGH);
	}

	// Run startup sequence.  A light display just for the fun of it.
	if (_configuration->runStartUpSequence)
	{
		startupSequence();
	}

	// Initial state.  Just to make sure.
	setGeneratorState(GENERATOR::OFF);

	// All ready, turn on "ready" indicator light.
	readyIndicatorLightOn();

	// If we are debugging, print that we are ready.
	debugPrintLn("Generator state initialized.", DEBUG::STANDARD);
}

// This is the main loop.  We keep it at light as possible by only updating when necessary.
void NaquadahGenerator::update()
{
	// Check the current state.
	GENERATOR::STATE newState = getGeneratorState();

	// If the current state is different than the set one, we update everything.  Otherwise, we don't update to save time.
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

			runSpecialMode();
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

		default:
		{
			// Error.
			debugPrint("Error in update, invalid state reached.", DEBUG::STANDARD);
			break;
		}
	}
}

Configuration* NaquadahGenerator::getConfiguration()
{
	return _configuration;
}

void NaquadahGenerator::readyIndicatorLightOn()
{
	digitalWrite(_configuration->readyIndicatorPin, LIGHT::ON);
}

void NaquadahGenerator::readyIndicatorLightOff()
{
	digitalWrite(_configuration->readyIndicatorPin, LIGHT::OFF);
}

void NaquadahGenerator::greenLightsOn()
{
	_shiftRegister.set(LIGHT::GREEN, LIGHT::ON);
}

void NaquadahGenerator::greenLightsOff()
{
	_shiftRegister.set(LIGHT::GREEN, LIGHT::OFF);
}

void NaquadahGenerator::redLightsOn()
{
	_shiftRegister.set(LIGHT::RED, LIGHT::ON);
}

void NaquadahGenerator::redLightsOff()
{
	_shiftRegister.set(LIGHT::RED, LIGHT::OFF);
}

void NaquadahGenerator::whiteLightsOn()
{
	_shiftRegister.set(LIGHT::WHITE, LIGHT::ON);
}

void NaquadahGenerator::whiteLightsOff()
{
	_shiftRegister.set(LIGHT::WHITE, LIGHT::OFF);
}

void NaquadahGenerator::blueLightsOn(unsigned int numberOfLights)
{
	// Number of lights provided has to be between 1 and 5.
	int lastOnLight = LIGHT::BLUE1 + numberOfLights - 1;

	// First set the state for each light.  We don't need to update every
	// time through the loop, so we use the no update version.
	// Set the "on" values.
	for (int i = LIGHT::BLUE1; i <= lastOnLight; i++)
	{
		_shiftRegister.setNoUpdate(i, LIGHT::ON);
	}
	// Set the "off" values.
	for (int i = lastOnLight + 1; i <= LIGHT::BLUE5; i++)
	{
		_shiftRegister.setNoUpdate(i, LIGHT::OFF);
	}

	// The states have been set, so call update now to do the update all at once.
	_shiftRegister.updateRegisters();
}

void NaquadahGenerator::blueLightsOff()
{
	for (int i = LIGHT::BLUE1; i <= LIGHT::BLUE5; i++)
	{
		// First set the state for each light.  We don't need to update every
		// time through the loop, so we use the no update version.
		_shiftRegister.setNoUpdate(i, LIGHT::OFF);
	}

	// The states have been set, so call update now to do the update all at once.
	_shiftRegister.updateRegisters();
}

// This does the main work of scrolling the blue lights.  If the lights are on, the current
// light is turned off and the next one turned on.  If the blue lights are off, a reset is done.
void NaquadahGenerator::incrementCurrentBlueLight()
{
	// All lights off (start with a clean slate).
	_shiftRegister.setNoUpdate(_currentBlueLight, LIGHT::OFF);

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

	_shiftRegister.set(_currentBlueLight, LIGHT::ON);

	_lightTimer.setTimeOutTime(_lightDelay);
	_lightTimer.reset();
}

void NaquadahGenerator::allLightsOff()
{
	greenLightsOff();
	redLightsOff();
	whiteLightsOff();
	blueLightsOff();
	readyIndicatorLightOn();
}

void NaquadahGenerator::blinkBlueLights(unsigned int numberOfLights)
{
	// This is to allow numbers more than 5 to be displayed.  Since we only have 5 blue lights,
	// we will blink 5 plus the remainder.  I.e., roller over means for than 5.
	bool rolledOver = false;

	unsigned int lightDelay = _configuration->startUpDelay;

	if (numberOfLights > 5)
	{
		rolledOver		= true;
		numberOfLights	= numberOfLights - 5;
	}

	for (int i = 0; i < 3; i++)
	{
		if (rolledOver)
		{
			// All five blue lights on to show the first 5.
			blueLightsOn(5);

			// Use a short dely to more closely associate the remainder with the first 5.  A longer
			// delay is used between the groups.
			delay(lightDelay);
		}

		delay(lightDelay);
		blueLightsOn(numberOfLights);
		delay(lightDelay);
		blueLightsOff();
	}
}

void NaquadahGenerator::rampBlueLightsOn(unsigned int delayBetweenLights)
{
	// Forwards on the blue lights.
	for (int i = LIGHT::BLUE1; i <= LIGHT::BLUE5; i++)
	{
		delay(delayBetweenLights);
		_shiftRegister.set(i, LIGHT::ON);
	}
}

void NaquadahGenerator::rampBlueLightsOff(unsigned int delayBetweenLights)
{
	for (int i = LIGHT::BLUE5; i >= LIGHT::BLUE1 ; i--)
	{
		delay(delayBetweenLights);
		_shiftRegister.set(i, LIGHT::OFF);
	}
}

void NaquadahGenerator::rampUpAllLights()
{
	// Forwards on the blue lights.
	rampBlueLightsOn(_configuration->startUpDelay);

	delay(2*_configuration->startUpDelay);
	greenLightsOn();

	// This function is only used when in the "Off" mode.  In this mode, the handle is covering the red
	// lights, so there is no reason to turn them on.  Save power by leaving them off.
	//redLightsOn();

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

void NaquadahGenerator::resetAll()
{
	resetLights();
	resetControls();
}

void NaquadahGenerator::resetControls()
{
	// Reset the toggle buttons so the initial state is active (off).
	_modeButton.reset();
	_modeButtonValue = GENERATOR::SPECIALMODEOFF;  
}

void NaquadahGenerator::resetLights()
{
	// Turn off all lights.
	allLightsOff();

	// We always want to start with standard delay.  Overload can only be created by first turning to ON, then
	// pressing the overload button.  We set the current blue light to 5 because we are going to call "increment"
	// to turn them on and increment with update to BLUE1 before turning on the light.
	_lightDelay       = _configuration->blueLightStandardDelay;
	_currentBlueLight = LIGHT::BLUE5;
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
	for (int i =  GENERATOR::OFF; i < GENERATOR::NUMBEROFSTATES; i++)
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

	// The state changes when a hall sensor is trigger, we want to make a sound to go with this event.
	// bool playResult = _vsUart.playFile("STATECHGOGG");
	// debugPrint("State change play: ", DEBUGLEVEL::STANDARD);
	// debugPrintLn((int)playResult, DEBUGLEVEL::STANDARD)
	_shiftRegister.set(AUDIO::ON, HIGH);
	_shiftRegister.set(AUDIO::STATECHANGE, LOW);
	
	// For the case of switching between PRIMED1 and ON, we don't want to turn off the red lights then turn
	// them back on.  Doing so might cause a flicker.  Therefore, we don't call reset when switching between
	// those two.
	switch (_generatorState)
	{
		case GENERATOR::OFF:
		{
			resetAll();
			break;
		}
			
		case GENERATOR::PRIMED0:
		{
			resetAll();
			greenLightsOn();
			break;
		}
			
		case GENERATOR::PRIMED1:
		{
			greenLightsOff();
			redLightsOn();
			whiteLightsOff();
			blueLightsOff();

			resetControls();
			break;
		}
			
		case GENERATOR::ON:
		{
			greenLightsOff();
			redLightsOn();
			whiteLightsOn();

			resetControls();

			// This will turn on the first light and start the timer.
			incrementCurrentBlueLight();

			delay(120);
			_shiftRegister.set(AUDIO::STATECHANGE, HIGH);
			_shiftRegister.set(AUDIO::ON, LOW);
			//_vsUart.playFile(F("NQHGENONOGG"));
			break;
		}

		case GENERATOR::NUMBEROFSTATES:
		{
			debugPrint("Error in setGeneratorState, invalid state reached.", DEBUG::STANDARD);
			break;
		}
	}

	delay(120);
	_shiftRegister.set(AUDIO::STATECHANGE, HIGH);
}

void NaquadahGenerator::setSpecialMode(GENERATOR::SPECIALMODE specialMode)
{
	debugPrint("Previous mode: ", DEBUG::STANDARD);
	debugPrintLn(_modeButtonValue, DEBUG::STANDARD);

	_modeButtonValue = specialMode;
	resetLights();

	debugPrint("Set special mode: ", DEBUG::STANDARD);
	debugPrintLn(_modeButtonValue, DEBUG::STANDARD);

	// Display which special mode we are in by blinking the corresponding number of blue lights.
	blinkBlueLights(_modeButtonValue);

	switch (_modeButtonValue)
	{
		case GENERATOR::SPECIALMODEOFF:
		{
			break;
		}

		case GENERATOR::SPECIALMODE01:
		{
			// This is the battery meter mode.  The battery meter has a timer in it to prevent flickering of the lights.  The update only
			// runs when the timer times out.  Normally, this works well, however we have a slightly different case.  We need to call
			// updateNow here to turn the lights on immediately without waiting for the timer.  In runSpecialMode, changes in the battery
			// level will be handled by the normal update function.
			_batteryMeter.updateNow();
			break;
		}

		case GENERATOR::SPECIALMODE02:
		{
			readyIndicatorLightOff();
			rampUpAllLights();
			readyIndicatorLightOn();
			break;
		}

		case GENERATOR::SPECIALMODE03:
		{
			blueLightsOn(5);
			break;
		}

		case GENERATOR::SPECIALMODE04:
		{
			whiteLightsOn();
			break;
		}

		case GENERATOR::SPECIALMODE05:
		{
			greenLightsOn();
			redLightsOn();
			break;
		}

		case GENERATOR::SPECIALMODE06:
		{
			// Test mode only.  Used to test total power draw from all lights.
			rampUpAllLights();
			redLightsOn();
			break;
		}

		case GENERATOR::NUMBEROFSPECIALMODES:
		{
			// The last enum is used as a placeholder to determine the total number of enums in the series.  However, the
			// Arduino IDE likes to complain when you don't handle all enums in the series so we use this to appease it.
			debugPrint("Error in setSpecialMode, invalid state reached.", DEBUG::STANDARD);
			break;
		}
	}
}

void NaquadahGenerator::runSpecialMode()
{
	debugPrint("Run special mode: ", DEBUG::STANDARD);
	debugPrintLn(_modeButtonValue, DEBUG::STANDARD);

	switch (_modeButtonValue)
	{
		case GENERATOR::SPECIALMODEOFF:
		{
			break;
		}

		case GENERATOR::SPECIALMODE01:
		{
			// This checkes the metering button and updates the blue lights accordingly.
			_batteryMeter.update();
			break;
		}

		case GENERATOR::SPECIALMODE02:
		case GENERATOR::SPECIALMODE03:
		case GENERATOR::SPECIALMODE04:
		case GENERATOR::SPECIALMODE05:
		case GENERATOR::SPECIALMODE06:
		{
			break;
		}

		case GENERATOR::NUMBEROFSPECIALMODES:
		{
			// The last enum is used as a placeholder to determine the total number of enums in the series.  However, the
			// Arduino IDE likes to complain when you don't handle all enums in the series so we use this to appease it.
			debugPrint("Error in runSpecialMode, invalid state reached.", DEBUG::STANDARD);
			break;
		}
	}
}

void  NaquadahGenerator::triggerAudio()
{
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