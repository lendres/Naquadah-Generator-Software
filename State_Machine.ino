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

// Input pins are defined in "pins.h."

// The current state (mode) of the device.
GENERATOR::STATE currentState       = GENERATOR::OFF;

// Stores the last state of the overload button.  This is used to note when the overload button changes
// state (low to high or high to low).  Since we are using the button as a toggle, we need to know that
// it has switched state.
int lastOverloadButtonState       = HIGH;

// Is the overload currently active?  Used to toggle overload on and off.
bool overloadActive               = false;

// We will greatly simplify this by assuming only one state can exist at a time and that state
// goes active when the associated input goes LOW.  This assumes the inputs cannot be activated
// at the same time, or at least a new active input overrides all currently active.  This implements
// the logic for the following behavior.
//
// Sensor 1 goes LOW: OFF (all lights off)
// Sensor 2 goes LOW: PRIMED0 (green lights on, all others off)
// Sensor 3 goes LOW: PRIMED1 (red lights on, all others off)
// Sensor 4 goes LOW: ON (red lights on, white lights on, blue lights SLOW scrolling, green lights off)
// Sensor 4 is LOW, activating sensor 5 (LOW then HIGH) toggles blues lights from scolling slow to fast and back (toggles between ON and OVERLOAD)

GENERATOR::STATE getGeneratorState(Configuration* configuration)
{
  // Start by finding the base state specified by when one of the Cap position sensors goes active.
  for (int i =  GENERATOR::OFF; i <= GENERATOR::ON; i++)
  {
    if (digitalRead(configuration->stateInputPins[i]) == LOW)
    {
      // We found the activated sensor, save it and break from the loop.
      currentState  = (GENERATOR::STATE)i;
      break;
    }
  }

  // Check to see if we should toggle the overloadActive status.  The first step is to check
  // to see if the overload button state has changed (high to low or low to high).
  if (digitalRead(configuration->stateInputPins[GENERATOR::OVERLOAD]) != lastOverloadButtonState)
  {
    // Overload button has changed state.
    // Save the new value.
    lastOverloadButtonState = digitalRead(configuration->stateInputPins[GENERATOR::OVERLOAD]);

    // We only handle the event where the button when from high to low (the button was pressed), the
    // release button event is ignored.
    if (lastOverloadButtonState == LOW)
    {
      // The button is has changed postion and is pressed (went from off to on).
      // Change state if the Cap is in the ON position.  In other states the button has no effect.
      if (currentState == GENERATOR::ON)
      {
        overloadActive = !overloadActive;
      }
    }
  }
  
  if (currentState == GENERATOR::ON && overloadActive)
  {
    currentState = GENERATOR::OVERLOAD;
  }
  else
  {
    // If we are not in the ON/OVERLOAD state, then overload is turned off.  If we are not in ON state, this resets
    // the overload to off (false).  This is so we never enter the ON state with overload active.
    overloadActive = false;
  }

  return currentState;
}
