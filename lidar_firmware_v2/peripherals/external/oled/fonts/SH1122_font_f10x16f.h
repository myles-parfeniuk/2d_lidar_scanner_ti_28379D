#ifndef SH1122_FONT_F10x16F_H_
#define SH1122_FONT_F10x16F_H_

//C standard library includes
#include <stdio.h>
//TI includes
#include <ti/sysbios/BIOS.h>
//in-house includes
#include "SH1122_char_def.h"

#define f10x16_FLOAT_WIDTH         10
#define f10x16_FLOAT_HEIGHT        16

#define f10x16f_CHAR_COUNT         95


char_map_t *f10x16f_decode_char(Uint8 character);

#endif /* SH1122_FONT_F10x16F_H_*/
