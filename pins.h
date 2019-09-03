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

const int numberOfStatePins   = 5;
const int stateInputPins[]    = {3, 4, 5, 6, 2};

const int dataPin             = 16;
const int clockPin            = 14;
const int latchPin            = 15;

// Blue top lights.
const int numberOfBlueLights  = 5;
const int blueLightPins[]     = {7, 8, 9, 10, 11};

// Cap, arm, and rail lights (red, green, and white lights).
const int greenLightPin       = 14;
const int redLightPin         = 15;
const int whiteLightPin       = 16;

// Green indicator light to indicate Arduino is ready.
const int readyIndicatorPin   = 12;

const int batteryPin          = 1;
const int batterySensePin     = A3;
