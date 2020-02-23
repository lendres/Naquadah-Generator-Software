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
    OVERLOAD,
    SPECIALMODE1,
    SPECIALMODE2
  };
}

// These are the light positions on the switch register.  The following rules must be followed:
// 1) The enum must be equivilant to the values 0-7.  I.e., don't try to assigned values to the enums.
// 2) The blue lights must be in order and consecutive.
namespace LIGHT
{
  enum POSITIONS
  {
    GREEN,
    RED,
    WHITE,
    BLUE1,
    BLUE2,
    BLUE3,
    BLUE4,
    BLUE5
  };

  enum STATE : uint8_t
  {
    OFF   = LOW,
    ON    = HIGH
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
