#include "SH1122_fonts.h"

char_map_t *decode_char(Uint8 character, font_def_t font)
{
    switch(font)
    {
        case font_f10x16f:
            return f10x16f_decode_char(character);

        case font_f6x8m:
            return f6x8_mono_decode_char(character);

        default:
            return NULL;
    }
}



