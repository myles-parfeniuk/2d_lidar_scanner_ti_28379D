#ifndef SH1122_OLED_H_
#define SH1122_OLED_H_

//C standard library includes
#include <math.h>
#include <stdarg.h>
//in-house includes
#include "../../internal/28379D_spi.h"
#include "SH1122_fonts.h"

//constants
#define OLED_WIDTH 256UL
#define OLED_HEIGHT 64UL
#define OLED_TILE_WIDTH 32
#define OLED_TILE_HEIGHT 8
//commands
#define OLED_CMD_POWER_ON 0xAF
#define OLED_CMD_POWER_OFF 0xAE
#define OLED_CMD_SET_ROW_ADDR 0xB0
#define OLED_CMD_SCAN_0_TO_N 0xC0 //scan rows from bottom to top
#define OLED_CMD_SCAN_N_TO_0 0xC8 //scan rows from top to bottom
#define OLED_CMD_NORM_SEG_MAP 0xA0 //regular segment driver output pad assignment
#define OLED_CMD_REV_SEG_MAP 0xA1 //reversed segment driver output pads
#define OLED_CMD_SET_MULTIPLEX_RATION 0xA8
#define OLED_CMD_SET_DC_DC_CONTROL_MOD 0xAD //onboard oled DC-DC voltage converter status and switch frequency
#define OLED_CMD_SET_OSCILLATOR_FREQ 0xD5
#define OLED_CMD_SET_DISP_START_LINE 0x40
#define OLED_CMD_SET_DISP_CONTRAST 0x81
#define OLED_CMD_SET_DISP_OFFSET_MOD 0xD3
#define OLED_CMD_SET_PRE_CHARGE_PERIOD 0xD9
#define OLED_CMD_SET_VCOM 0xDB
#define OLED_CMD_SET_VSEG 0xDC
#define OLED_CMD_SET_DISCHARGE_LEVEL 0x30
//macros
#define OLED_DE_ASSRT_RST GpioDataRegs.GPBSET.bit.GPIO59 = 1
#define OLED_ASSRT_RST    GpioDataRegs.GPBCLEAR.bit.GPIO59 = 1

#define OLED_ASSRT_DATA GpioDataRegs.GPDSET.bit.GPIO124 = 1
#define OLED_ASSRT_CMD    GpioDataRegs.GPDCLEAR.bit.GPIO124 = 1

//colors
typedef enum
{
    oled_intens_0 = 0x00,
    oled_intens_1 = 0x11,
    oled_intens_2 = 0x22,
    oled_intens_3 = 0x33,
    oled_intens_4 = 0x44,
    oled_intens_5 = 0x55,
    oled_intens_7 = 0x77,
    oled_intens_8 = 0x88,
    oled_intens_9 = 0x99,
    oled_intens_10 = 0xAA,
    oled_intens_11 = 0xBB,
    oled_intens_12 = 0xCC,
    oled_intens_13 = 0xDD,
    oled_intens_14 = 0xEE,
    oled_intens_15 = 0xFF

} intensity_t;

void oled_init();
//config functions
void oled_set_orientation(bool flipped);

//draw functions
void oled_set_pixel(uint16_t x, uint16_t y, Uint8 intensity);
void oled_draw_line(int16_t x_1, int16_t y_1, int16_t x_2, int16_t y_2, intensity_t intensity);
void oled_draw_frame(int16_t x_1, int16_t y_1, int16_t x_2, int16_t y_2, intensity_t intensity);
void oled_draw_rectangle(int16_t x_1, int16_t y_1, int16_t x_2, int16_t y_2, intensity_t intensity);
void oled_draw_char(int16_t x_1, int16_t y_1, char_map_t *map, intensity_t intensity);
void oled_draw_string(int16_t x_1, int16_t y_1, intensity_t intensity, font_def_t font, const char *format, ...);

void oled_send_buffer();
void oled_clear_buffer();
void oled_clear_RAM();

static void oled_send_cmd(Uint8 *cmd, Uint8 length);
static void oled_reset();
static void oled_write_data(Uint8 *data, uint32_t length);
//commands
static void oled_cmd_power_on();
static void oled_cmd_power_off();


//frame buffer
static Uint8 frame_buffer[OLED_WIDTH][OLED_HEIGHT] = {0};
static Uint8 send_buffer[OLED_WIDTH * OLED_HEIGHT / 2] = {0};



#endif /* SH1122_OLED_H_ */
