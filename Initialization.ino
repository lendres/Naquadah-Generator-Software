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

// Initialize the pins for the top red and green lights, the arm white lights,
// the front and back rail white lights and the "ready" indicator light.
void initializeStaticLightPins()
{
   pinMode(readyIndicatorPin, OUTPUT);
   pinMode(greenLightPin, OUTPUT);
   pinMode(redLightPin, OUTPUT);
   pinMode(whiteLightPin, OUTPUT);
}

// Initialize the blue light pins.
void initializeBlueLightPins()
{
  // Set up blue top lights as output pins.
  for (int i = 0; i < numberOfBlueLights; i++)
  {
    pinMode(blueLightPins[i], OUTPUT);
  }

  // Initialize to off.
  setBlueLightsState(LOW);
}

// Inputs for hall effect sensors and overload button.
void initializeInputPins()
{
  // Set all the state pins as input.
  for (int i = 0; i < numberOfStatePins; i++)
  {
    // Set as input (read from them).
    pinMode(stateInputPins[i], INPUT);
    
    // Use internal resistor to pull pin to high.  They are pulled low to indicate
    // activation.
    digitalWrite(stateInputPins[i], HIGH);
  }

  // Battery sensor.  We need it low to detect voltage above ground as battery level.
  pinMode(batterySensePin, INPUT);
  pinMode(batterySensePin, LOW);
}

// Do a cool startup.
void startupSequence()
{
  // Forwards on the blue lights.
  for (int i = 0; i < numberOfBlueLights; i++)
  {
    delay(startUpDelay);
    digitalWrite(blueLightPins[i], HIGH);
  }

  delay(2*startUpDelay);
  digitalWrite(greenLightPin, HIGH);
  digitalWrite(redLightPin, HIGH);

  delay(2*startUpDelay);
  digitalWrite(whiteLightPin, HIGH);

  // Pause with all the lights on.
  delay(12*startUpDelay);
  
  digitalWrite(whiteLightPin, LOW);

  delay(2*startUpDelay);
  digitalWrite(greenLightPin, LOW);
  digitalWrite(redLightPin, LOW);

  delay(startUpDelay);

  // Backwards on the blue lights.
  for (int i = numberOfBlueLights; i > -1 ; i--)
  {
    delay(startUpDelay);
    digitalWrite(blueLightPins[i], LOW);
  }
}
