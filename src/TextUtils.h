/* N64 GAME ENGINE */
// Text utilities header
// Written by MEMESCOEP
// November of 2024
// Thanks to the LibDragon and Tiny3D libraries for making this project possible
// LibDragon github -> https://github.com/DragonMinded/libdragon
// Tiny3D github -> https://github.com/HailToDodongo/tiny3d


// Define TEXTUTILS_H if it hasn't been already
#ifndef TEXTUTILS_H
#define TEXTUTILS_H


/* LIBRARIES */
#include "N64GameEngine.h"


/* FUNCTIONS */
// ----- Registration functions -----
rdpq_font_t* RegisterFontBasic(char* FontPath, color_t TextColor, color_t OutlineColor, int FontID);
rdpq_font_t* RegisterFontWithStyle(char* FontPath, int FontID, rdpq_fontstyle_t* FontStyle);

// ----- Measurement functions -----
int PixelStrHeighjt(char* String, int FontID);
int PixelStrWidth(char* String, int FontID);
#endif