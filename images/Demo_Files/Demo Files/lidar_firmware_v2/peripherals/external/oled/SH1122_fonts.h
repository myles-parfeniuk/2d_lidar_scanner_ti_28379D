#ifndef _OLED_FONTS_SH1122_FONTS_H_
#define _OLED_FONTS_SH1122_FONTS_H_

#include "fonts/SH1122_font_f10x16f.h"
#include "fonts/SH1122_font_f6x8_mono.h"
/***********************************************************************************
 * SSH1122_fonts.h
 *
 * Module used to decode chars when writing string to OLED display. Uses the correct
 * look-up table based off which font is passed to draw function.
 *
 * Author:    Myles Parfeniuk
 * Date:      10/09/2023
 * Modified:  10/09/2023
 *********************************************************************************/

typedef enum
{
    font_f10x16f,
    font_f6x8m

} font_def_t;

char_map_t *decode_char(Uint8 character, font_def_t font);

#endif /*SH1122_FONT_F10x16F_H_*/
