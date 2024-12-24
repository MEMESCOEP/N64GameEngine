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


/* VARIABLES */
struct HSVColor
{
    float H;
    float S;
    float V;
};


/* FUNCTIONS */
// ----- Conversion functions -----
void RGBAToHSV(color_t ColorToConvert, struct HSVColor* HSVStruct);
void HSVToRGBA(struct HSVColor HSVToConvert, color_t* Color);

// ----- Manipulation functions -----
color_t GetRainbowColor(float Sin, int Alpha);
void LerpColor(color_t* ColorToLerp, color_t InitialColor, color_t TargetColor, float Time);
void FadeAlpha(color_t* ColorToFade, int AlphaValue, float Time);

// ----- Compare functions -----
bool AreRGBAColorsEqual(color_t Color1, color_t Color2);
bool AreRGBColorsEqual(color_t Color1, color_t Color2);
bool AreHSVColorsEqual(struct HSVColor Color1, struct HSVColor Color2);
#endif