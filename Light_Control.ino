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

// Sets the state of the top red and green cap lights and the arm and rail white lights.
void setStaticLights(GENERATORSTATE currentState)
{ 
  switch (currentState)
  {
    case OFF:
      digitalWrite(greenLightPin, LOW);
      digitalWrite(redLightPin, LOW);
      digitalWrite(whiteLightPin, LOW);
      break;
      
    case PRIMED0:
      digitalWrite(greenLightPin, HIGH);
      digitalWrite(redLightPin, LOW);
      digitalWrite(whiteLightPin, LOW);
      break;
      
    case PRIMED1:
      digitalWrite(greenLightPin, LOW);
      digitalWrite(redLightPin, HIGH);
      digitalWrite(whiteLightPin, LOW);
      break;
      
    case ON:
      digitalWrite(greenLightPin, LOW);
      digitalWrite(redLightPin, HIGH);
      digitalWrite(whiteLightPin, HIGH);
      break;
      
    case OVERLOAD:
      digitalWrite(greenLightPin, LOW);
      digitalWrite(redLightPin, HIGH);
      digitalWrite(whiteLightPin, HIGH);
      break;
  }
}

// Sets the time delay used for blue light scrolling.
void setBlueLightDelay(GENERATORSTATE currentState)
{
  // We have one delay for overload mode and one for standard mode.
  if (currentState == OVERLOAD)
  {
    blueLightDelay = blueLightOverloadDelay;
  }
  else
  {
    blueLightDelay = blueLightStandardDelay;
  }
}

// This does the main work of scrolling the blue lights.  If the lights are on, the current
// light is turned off and the next one turned on.  If the blue lights are off, a reset is done.
void incrementCurrentBlueLight(GENERATORSTATE currentState)
{
  // All lights off (start with a clean slate).
  setBlueLightsState(LOW);

  // If we are at the last light, reset to the first.
  if (currentBlueLight == numberOfBlueLights)
  {
    currentBlueLight = 0;
  }

  // Turn on the appropriate light or reset for next time.
  switch (currentState)
  {
    case ON:
    case OVERLOAD:
      // Turn on the light and increment the light counter for next time.
      digitalWrite(blueLightPins[currentBlueLight++], HIGH);
      break;
      
    default:
      // If we are not in ON or OVERLOAD, reset so we start from the first one next time.
      currentBlueLight = 0;
      break;
  }
}

// Sets all blue lights to the specified state (HIGH or LOW).
void setBlueLightsState(uint8_t state)
{
  for (int i = 0; i < numberOfBlueLights; i++)
  {
    digitalWrite(blueLightPins[i], state);
  }
}
