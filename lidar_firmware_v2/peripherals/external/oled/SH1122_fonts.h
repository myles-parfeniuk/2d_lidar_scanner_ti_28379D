#ifndef _OLED_FONTS_SH1122_FONTS_H_
#define _OLED_FONTS_SH1122_FONTS_H_

#include "fonts/SH1122_font_f10x16f.h"
#include "fonts/SH1122_font_f6x8_mono.h"

typedef enum
{
    font_f10x16f,
    font_f6x8m

} font_def_t;

char_map_t *decode_char(Uint8 character, font_def_t font);

#endif /*SH1122_FONT_F10x16F_H_*/
