#ifndef OLED_FONTS_SH1122_CHAR_DEF_H_
#define OLED_FONTS_SH1122_CHAR_DEF_H_

/***********************************************************************************
 * SH1122_char_def.h
 *
 * Header containing definition for struct used by character look up tables in SH1122_font_* files.
 *
 * Author:    Myles Parfeniuk
 * Date:      10/09/2023
 * Modified:  10/09/2023
 *********************************************************************************/

typedef struct char_map_t{
    Uint8 character; //the character associated with bitmapped contained in struct
    Uint8 width; //width of the character
    Uint8 height; //height of the character
    const Uint8 bit_map[32]; //bit map of the character
} char_map_t;


#endif /* PERIPHERALS_EXTERNAL_OLED_FONTS_SH1122_CHAR_DEF_H_ */
