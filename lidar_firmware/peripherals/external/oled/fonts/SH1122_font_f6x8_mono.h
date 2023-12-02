#ifndef SH1122_FONT_F6X8_MONO_H_
#define SH1122_FONT_F6X8_MONO_H_

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
