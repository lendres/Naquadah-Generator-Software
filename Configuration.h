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
	// DEBUGGING.
	// If true (1) debugging messages are printed.
	const DEBUG::DEBUGLEVEL   DebugLevel                   = DEBUG::STANDARD;

	// INPUT.
	// Just set the input pins the 4 hall effect sensors is on.  The number of state pins
	// should not be changed.
	const int                 numberOfStatePins           = 4;
	const int*                stateInputPins              = new int[numberOfStatePins] {3, 4, 5, 6};

	// The pin the mode button is connected to.
	const int                 modeButtonPin               = 2;

	// OUTPUT.
	// Output shift register pins.
	const int                 shiftRegisterDataPin        = 14;
	const int                 shiftRegisterClockPin       = 15;
	const int                 shiftRegisterLatchPin       = 16;
	
	 // Green indicator light to indicate Arduino is ready.
	const int                 readyIndicatorPin           = 12;

	// CHARGER/BOOSTER ACTIVATION
	// Some chargers/boosters power down if you don't draw power from them.  Some have a
	// button used to power them on.  This is used to activate the button.
	const bool                useChargerKey               = true;
	const int                 chargerKeyPin               = 10;

	// Delay between re-activation calls.
	const unsigned int        chargerKeyDelay             = 10000;

	// Time the button is pushed (activation length).
	const unsigned int        chargerKeyActivationTime    = 500;

	// BATTERY METER SETTINGS.
	// The pin the activation button is on.
	unsigned int              batteryMeterActivationPin   = 7;

	// The pin that is used to sense the battery voltage.
	unsigned int              batteryMeterSensePin        = A3;
	
	// Set the min and max reading values that correspond to 2.7 and 4.2 volts (for a lithium battery).
	unsigned int              batteryMinReading           = 646;
	unsigned int              batteryMaxReading           = 865;

	// BEHAVIOR SETTINGS.
	// Values for timing.
	const unsigned int        blueLightStandardDelay      = 130;
	const unsigned int        blueLightOverloadIncrement  = 30;
	const unsigned int        startUpDelay                = 1.5*blueLightStandardDelay;

	// Start up sequence.
	const bool                runStartUpSequence          = false;
};

#endif
