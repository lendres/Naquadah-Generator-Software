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

void rampUpLights(NaquadahGenerator* naquadahGenerator)
{
  // Forwards on the blue lights.
  naquadahGenerator->rampBlueLightsOn(naquadahGenerator->getConfiguration()->startUpDelay);

  delay(2*naquadahGenerator->getConfiguration()->startUpDelay);
  naquadahGenerator->greenLightsOn();
  naquadahGenerator->redLightsOn();

  delay(2*naquadahGenerator->getConfiguration()->startUpDelay);
  naquadahGenerator->whiteLightsOn();
}

void rampDownLights(NaquadahGenerator* naquadahGenerator)
{
  naquadahGenerator->whiteLightsOff();

  delay(2*naquadahGenerator->getConfiguration()->startUpDelay);
  naquadahGenerator->greenLightsOff();
  naquadahGenerator->redLightsOff();

  // Backwards on the blue lights.
  delay(naquadahGenerator->getConfiguration()->startUpDelay);
  naquadahGenerator->rampBlueLightsOff(naquadahGenerator->getConfiguration()->startUpDelay);
}

// Do a cool startup.
void startupSequence(NaquadahGenerator* naquadahGenerator)
{
  rampUpLights(naquadahGenerator);

  // Pause with all the lights on.
  delay(12*naquadahGenerator->getConfiguration()->startUpDelay);

  rampDownLights(naquadahGenerator);
}

void specialModeOne(NaquadahGenerator* naquadahGenerator)
{
  // All ready, turn on "ready" indicator light.
  digitalWrite(naquadahGenerator->getConfiguration()->readyIndicatorPin, LOW);

  rampUpLights(naquadahGenerator);

  // All ready, turn on "ready" indicator light.
  digitalWrite(naquadahGenerator->getConfiguration(), HIGH);
}

void specialModeTwo(NaquadahGenerator* naquadahGenerator)
{
  
}
