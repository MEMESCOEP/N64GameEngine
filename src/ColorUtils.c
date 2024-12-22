/* N64 GAME ENGINE */
// Color utilities file
// Written by MEMESCOEP
// December of 2024
// Thanks to the LibDragon and Tiny3D libraries for making this project possible
// LibDragon github -> https://github.com/DragonMinded/libdragon
// Tiny3D github -> https://github.com/HailToDodongo/tiny3d


/* LIBRARIES */
#include "N64GameEngine.h"
#include "ColorUtils.h"
#include "MathUtils.h"


/* FUNCTIONS */
// Get a rainbow color from a sine value
color_t GetRainbowColor(float Sin, int Alpha)
{
    return RGBA32(
        fm_sinf(Sin + 0.0f) * 127.0f + 128.0f,
        fm_sinf(Sin + 2.0f) * 127.0f + 128.0f,
        fm_sinf(Sin + 4.0f) * 127.0f + 128.0f,
        Alpha
    );
}

// Change from one color (RGBA) to another over time
void LerpColor(color_t* ColorToLerp, color_t EndColor, float Time)
{
    ColorToLerp->r = LerpInt(ColorToLerp->r, EndColor.r, Time);
    ColorToLerp->g = LerpInt(ColorToLerp->g, EndColor.g, Time);
    ColorToLerp->b = LerpInt(ColorToLerp->b, EndColor.b, Time);
    ColorToLerp->a = LerpInt(ColorToLerp->a, EndColor.a, Time);
}

// Fade the RGBA color's alpha value in or out over time
void FadeAlpha(color_t* ColorToFade, int AlphaValue, float Time)
{
    ColorToFade->a = LerpInt(ColorToFade->a, AlphaValue, Time);
}

// Check if the RGBA values of two colors are equal
bool AreColorsEqual(color_t Color1, color_t Color2)
{
    return Color1.r == Color2.r &&
        Color1.g == Color2.g &&
        Color1.b == Color2.b &&
        Color1.a == Color2.a;
}