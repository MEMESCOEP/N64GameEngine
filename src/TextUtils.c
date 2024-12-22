/* N64 GAME ENGINE */
// Text utilities file
// Written by MEMESCOEP
// December of 2024
// Thanks to the LibDragon and Tiny3D libraries for making this project possible
// LibDragon github -> https://github.com/DragonMinded/libdragon
// Tiny3D github -> https://github.com/HailToDodongo/tiny3d


/* LIBRARIES */
#include <string.h>
#include "N64GameEngine.h"
#include "TextUtils.h"


/* FUNCTIONS */
// ----- Registration functions -----
// Registers a font to the specified font ID, with a custom color
rdpq_font_t* RegisterFontBasic(char* FontPath, color_t TextColor, color_t OutlineColor, int FontID)
{
    rdpq_font_t *NewFont = rdpq_font_load(FontPath);
    rdpq_font_style(NewFont, 0, &(rdpq_fontstyle_t){
        .color = TextColor,
        .outline_color = OutlineColor,
    });

    rdpq_text_register_font(FontID, NewFont);
    return NewFont;
}

// Registers a font to the specified font ID, with the specified style
rdpq_font_t* RegisterFontWithStyle(char* FontPath, int FontID, rdpq_fontstyle_t* FontStyle)
{
    rdpq_font_t *NewFont = rdpq_font_load(FontPath);
    rdpq_font_style(NewFont, 0, FontStyle);
    rdpq_text_register_font(FontID, NewFont);
    return NewFont;
}

// ----- Measurement functions -----
// Returns the pixel height of a string
int PixelStrHeight(char* String, int FontID)
{
    int CharNum = strlen(String);
    rdpq_paragraph_t* NewParagraph = rdpq_paragraph_build(NULL, FontID, String, &CharNum);
    int PixelHeight = NewParagraph->bbox.y1 - NewParagraph->bbox.y0;

    rdpq_paragraph_free(NewParagraph);
    NewParagraph = NULL;

    return PixelHeight;
}

// Returns the pixel width of a string
int PixelStrWidth(char* String, int FontID)
{
    int CharNum = strlen(String);
    rdpq_paragraph_t* NewParagraph = rdpq_paragraph_build(NULL, FontID, String, &CharNum);
    int PixelWidth = NewParagraph->bbox.x1 - NewParagraph->bbox.x0;

    rdpq_paragraph_free(NewParagraph);
    NewParagraph = NULL;

    return PixelWidth;
}