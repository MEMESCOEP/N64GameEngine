/* N64 GAME ENGINE */
// Color utilities header
// Written by MEMESCOEP
// November of 2024
// Thanks to the LibDragon and Tiny3D libraries for making this project possible
// LibDragon github -> https://github.com/DragonMinded/libdragon
// Tiny3D github -> https://github.com/HailToDodongo/tiny3d


// Define COLORUTILS_H if it hasn't been already
#ifndef COLORUTILS_H
#define COLORUTILS_H


/* LIBRARIES */
#include "N64GameEngine.h"


/* DEFINITIONS */


/* FUNCTIONS */
color_t GetRainbowColor(float Sin, int Alpha);
void LerpColor(color_t* ColorToLerp, color_t EndColor, float Time);
void FadeAlpha(color_t* ColorToFade, int AlphaValue, float Time);
bool AreColorsEqual(color_t Color1, color_t Color2);
#endif