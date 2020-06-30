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
#include "ShiftRegister74HC595.h"
#include "BatteryMeterShiftRegister.h"
#include "MomentaryButton.h"
#include "CycleButton.h"
#include "SoftTimers.h"
#include "BlinkShiftRegister.h"

#include "BlinkPin.h"

class NaquadahGenerator
{
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
		// Standard on/off/increment (blue lights) lighting control functions.
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

		void allLightsOff();

		// Lights functions for sequences, special modes, et cetera.
		void rampBlueLightsOn(unsigned int delayBetweenLights);
		void rampBlueLightsOff(unsigned int delayBetweenLights);

		void rampUpLights();
		void rampDownAllLights();

		void startupSequence();
		
	private:
		// Initialization functions.
		void initializeBatteryMeter();

		// Reset functions.
		void resetAll();
		void resetLights();
		void resetControls();

		// Generator state.
		GENERATOR::STATE getGeneratorState();
		void setGeneratorState(GENERATOR::STATE state);

		void setSpecialMode(GENERATOR::SPECIALMODE specialMode);
		void runSpecialMode();

		// Debug messages.
		void debugPrint(const char message[], DEBUG::DEBUGLEVEL level);
		void debugPrint(int message, DEBUG::DEBUGLEVEL level);
		
		void debugPrintLn(const char message[], DEBUG::DEBUGLEVEL level);
		void debugPrintLn(int message, DEBUG::DEBUGLEVEL level);
	 		
	private:
		// Arduino pin and control settings.
		Configuration*										_configuration;
		
		// Output.  Because of the number of outputs, a shift register is used.
		ShiftRegister74HC595<nShiftRegisters>				_shiftRegister;

		// Battery meter.
		BatteryMeterShiftRegister<nShiftRegisters>			_batteryMeter;
		MomentaryButton										_batteryMeterButton;

		// Virtual cycle button for special modes.
		CycleButton											_modeButton;
		GENERATOR::SPECIALMODE								_modeButtonValue;

		// The current state of the generator.  This is the activation arm position.
		GENERATOR::STATE									_generatorState;

		// This is the currently active blue light.  It is used to be able to scroll the lights.
		unsigned int										_currentBlueLight;
		
		// Variable to hold current delay we are using.  This specifies how often the lights are changed in
		// modes where you have blinking, scrolling, et cetera lights.
		unsigned int                   						_lightDelay;

		// Timer used to determine when to update blue lights and without blocking code execution with "delay."
		SoftTimer											_lightTimer;

		// Timer used to keep charger/booster active, if required.  Some boards shut off if the power draw
		// is low.  This is used to keep them on in the GENERATOR::OFF state where power use is low.
// BlinkShiftRegister<nShiftRegisters>					_chargerKeyBlinker;
};

#endif