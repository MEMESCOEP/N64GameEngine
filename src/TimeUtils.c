/* N64 GAME ENGINE */
// Time utilities file
// Written by MEMESCOEP
// November of 2024
// Thanks to the LibDragon and Tiny3D libraries for making this project possible
// LibDragon github -> https://github.com/DragonMinded/libdragon
// Tiny3D github -> https://github.com/HailToDodongo/tiny3d


/* LIBRARIES */
#include "N64GameEngine.h"
#include "TimeUtils.h"


/* FUNCTIONS */
// ----- Framerate / frame time math -----
// Calculate milliseconds per frame for a given framerate
float MSPFFromFPS(int FPSToConvert)
{
    return FPSToMS(FPSToConvert) / FPSToConvert;
}

// Determines the number of milliseconds in a given framerate
float FPSToMS(int FPSToConvert)
{
    return TIMER_MICROS_LL(N64ClockSpeed / FPSToConvert) / 1000.0f;
}


// ----- Timer helpers -----
// Convert milliseconds to timer ticks
float MSToTicks(int MS)
{
    return TIMER_TICKS_LL(MS * 1000.0f);
}