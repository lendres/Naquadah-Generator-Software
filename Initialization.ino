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

// Do a cool startup.
void startupSequence(NaquadahGenerator* naquadahGenerator)
{
  // Forwards on the blue lights.
  naquadahGenerator->rampBlueLightsOn(configuration.startUpDelay);

  delay(2*configuration.startUpDelay);
  naquadahGenerator->greenLightsOn();
  naquadahGenerator->redLightsOn();

  delay(2*configuration.startUpDelay);
  naquadahGenerator->whiteLightsOn();

  // Pause with all the lights on.
  delay(12*configuration.startUpDelay);
  
  naquadahGenerator->whiteLightsOff();

  delay(2*configuration.startUpDelay);
  naquadahGenerator->greenLightsOff();
  naquadahGenerator->redLightsOff();

  // Backwards on the blue lights.
  delay(configuration.startUpDelay);
  naquadahGenerator->rampBlueLightsOff(configuration.startUpDelay);
}
