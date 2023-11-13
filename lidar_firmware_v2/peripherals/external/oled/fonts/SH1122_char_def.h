#ifndef OLED_FONTS_SH1122_CHAR_DEF_H_
#define OLED_FONTS_SH1122_CHAR_DEF_H_

typedef struct char_map_t{
    Uint8 character;
    Uint8 width;
    Uint8 height;
    const Uint8 bit_map[32];
} char_map_t;


#endif /* PERIPHERALS_EXTERNAL_OLED_FONTS_SH1122_CHAR_DEF_H_ */
