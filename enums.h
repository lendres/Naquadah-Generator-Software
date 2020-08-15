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

#ifndef ENUMS_H
#define ENUMS_H

namespace GENERATOR
{
	enum STATE
	{
		OFF,
		PRIMED0,
		PRIMED1,
		ON,
		NUMBEROFSTATES
	};

	enum SPECIALMODE
	{
		SPECIALMODEOFF,
		SPECIALMODE01,
		SPECIALMODE02,
		SPECIALMODE03,
		SPECIALMODE04,
		SPECIALMODE05,
		SPECIALMODE06,
		NUMBEROFSPECIALMODES
	};
}

// These are the light positions on the shift register.  The following rules must be followed:
// 1) The enum must start at zero and be consecutive.  I.e., don't try to assign values to the enums.
// 2) The blue lights must be in order and consecutive.
namespace LIGHT
{
	// These lights are connected to a shift register.  The value of the enumeration
	// is the postion on the shift register.
	enum SHIFTREGISTER
	{

		RED,
		BLUE1,
		BLUE2,
		BLUE3,
		BLUE4,
		BLUE5,
		WHITE,
		GREEN,
		READY
	};

	enum STATE : uint8_t
	{
		OFF   = LOW,
		ON    = HIGH
	};
}

// These are the other (non-light) outputs.  These and the light positions need to be consecutive when
// using the shift registers for output.
namespace AUDIO
{
	enum SHIFTREGISTER
	{
		UG = LIGHT::READY + 1,
		RESET,
		STATECHANGE,
		ON
	};
}

namespace DEBUG
{
	enum DEBUGLEVEL
	{
		OFF,
		STANDARD,
		VERBOSE
	};
}

#endif