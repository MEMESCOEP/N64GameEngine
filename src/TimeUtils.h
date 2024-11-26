/* N64 GAME ENGINE */
// Time utilities header
// Written by MEMESCOEP
// November of 2024
// Thanks to the LibDragon and Tiny3D libraries for making this project possible
// LibDragon github -> https://github.com/DragonMinded/libdragon
// Tiny3D github -> https://github.com/HailToDodongo/tiny3d


// Define TIMEUTILS_H if it hasn't been already
#ifndef TIMEUTILS_H
#define TIMEUTILS_H


/* LIBRARIES */
#include "N64GameEngine.h"


/* DEFINITIONS */
#define N64ClockSpeed 93750000


/* FUNCTIONS */
// ----- Framerate / frame time math -----
// Calculate milliseconds per frame for a given framerate
float MSPFFromFPS(int FPSToConvert);

// Determines the number of milliseconds in a given framerate
float FPSToMS(int FPSToConvert);


// ----- Timer helpers -----
// Convert milliseconds to timer ticks
float MSToTicks(int MS);
#endif