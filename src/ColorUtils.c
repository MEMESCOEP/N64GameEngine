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
// ----- Conversion functions -----
// Convert an RGB or RGBA color to the HSV color space
void RGBAToHSV(color_t ColorToConvert, struct HSVColor* HSVStruct)
{
    // Normalize RGB values to the range (0 - 1)
    float RNorm = ColorToConvert.r / 255.0f;
    float GNorm = ColorToConvert.g / 255.0f;
    float BNorm = ColorToConvert.b / 255.0f;
    float Cmax = RNorm > GNorm ? (RNorm > BNorm ? RNorm : BNorm) : (GNorm > BNorm ? GNorm : BNorm);
    float Cmin = RNorm < GNorm ? (RNorm < BNorm ? RNorm : BNorm) : (GNorm < BNorm ? GNorm : BNorm);
    float Delta = Cmax - Cmin;

    // H calculation (degrees, 0 - 360)
    if (Delta == 0)
    {
        HSVStruct->H = 0;
    }
    else if (Cmax == RNorm)
    {
        HSVStruct->H = 60 * fmod(((GNorm - BNorm) / Delta), 6);
    }
    else if (Cmax == GNorm)
    {
        HSVStruct->H = 60 * (((BNorm - RNorm) / Delta) + 2);
    }
    else if (Cmax == BNorm)
    {
        HSVStruct->H = 60 * (((RNorm - GNorm) / Delta) + 4);
    }

    if (HSVStruct->H < 0)
    {
        HSVStruct->H += 360;
    }

    // S & V calculation (percentage, 0 - 100)
    if (Cmax == 0)
    {
        HSVStruct->S = 0;
    }
    else
    {
        HSVStruct->S = Delta / Cmax;
    }

    HSVStruct->V = Cmax;
}

// Convert a HSV color to the RGBA color space
void HSVToRGBA(struct HSVColor HSVToConvert, color_t* Color)
{
    float RPrime, GPrime, BPrime;
    float Chroma = HSVToConvert.V * HSVToConvert.S;
    float Match = HSVToConvert.V - Chroma;
    float HPos = Chroma * (1 - fabs(fmod(HSVToConvert.H / 60.0, 2) - 1));

    if (HSVToConvert.H >= 0 && HSVToConvert.H < 60)
    {
        RPrime = Chroma;
        GPrime = HPos;
        BPrime = 0;
    }
    else if (HSVToConvert.H >= 60 && HSVToConvert.H < 120)
    {
        RPrime = HPos;
        GPrime = Chroma;
        BPrime = 0;
    }
    else if (HSVToConvert.H >= 120 && HSVToConvert.H < 180)
    {
        RPrime = 0;
        GPrime = Chroma;
        BPrime = HPos;
    }
    else if (HSVToConvert.H >= 180 && HSVToConvert.H < 240)
    {
        RPrime = 0;
        GPrime = HPos;
        BPrime = Chroma;
    }
    else if (HSVToConvert.H >= 240 && HSVToConvert.H < 300)
    {
        RPrime = HPos;
        GPrime = 0;
        BPrime = Chroma;
    }
    else
    {
        RPrime = Chroma;
        GPrime = 0;
        BPrime = HPos;
    }

    Color->r = (int)((RPrime + Match) * 255);
    Color->g = (int)((GPrime + Match) * 255);
    Color->b = (int)((BPrime + Match) * 255);
}

// ----- Manipulation functions -----
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
void LerpColor(color_t* ColorToLerp, color_t InitialColor, color_t TargetColor, float Time)
{
    struct HSVColor InitialHSV, TargetHSV, CurrentHSV;

    RGBAToHSV(InitialColor, &InitialHSV);
    RGBAToHSV(TargetColor, &TargetHSV);

    CurrentHSV.H = LerpFloat(InitialHSV.H, TargetHSV.H, Time);
    CurrentHSV.S = LerpFloat(InitialHSV.S, TargetHSV.S, Time);
    CurrentHSV.V = LerpFloat(InitialHSV.V, TargetHSV.V, Time);

    HSVToRGBA(CurrentHSV, ColorToLerp);
}

// Fade the RGBA color's alpha value in or out over time
void FadeAlpha(color_t* ColorToFade, int AlphaValue, float Time)
{
    ColorToFade->a = LerpInt(ColorToFade->a, AlphaValue, Time);
}

// ----- Compare functions -----
// Check if the RGBA values of two colors are equal
bool AreRGBAColorsEqual(color_t Color1, color_t Color2)
{
    return AreRGBColorsEqual(Color1, Color2) && Color1.a == Color2.a;
}

// Check if the RGB values of two colors are equal
bool AreRGBColorsEqual(color_t Color1, color_t Color2)
{
    return Color1.r == Color2.r &&
        Color1.g == Color2.g &&
        Color1.b == Color2.b;
}

// Check if the HSV values of two colors are equal
bool AreHSVColorsEqual(struct HSVColor Color1, struct HSVColor Color2)
{
    return Color1.H == Color2.H &&
        Color1.S == Color2.S &&
        Color1.V == Color2.V;
}