#ifndef SH1122_FONT_F6X8_MONO_H_
#define SH1122_FONT_F6X8_MONO_H_

/***********************************************************************************
 * SH1122_font_f6x8_mono.h
 *
 * OLED font definition for 6x8 pixel mono. Fonts were modified with custom python script
 * from originals to change formatting to something possible to build with our the
 * F28397D. Decode char implemented to grab desired character data without use of for loop.
 *
 * Author:    Myles Parfeniuk
 * Date:      10/09/2023
 * Modified:  10/09/2023
 *********************************************************************************/

//C standard library includes
#include <stdio.h>
//TI includes
#include <ti/sysbios/BIOS.h>
//in-house includes
#include "SH1122_char_def.h"

#define f6x8_MONO_WIDTH       6
#define f6x8_MONO_HEIGHT      8

#define f6x8_CHAR_COUNT       95

char_map_t *f6x8_mono_decode_char(Uint8 character);

#endif /*SH1122_FONT_F6X8_MONO_H_*/
