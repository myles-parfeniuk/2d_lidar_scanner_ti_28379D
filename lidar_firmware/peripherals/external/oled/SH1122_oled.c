//in-house includes
#include "SH1122_oled.h"

//C standard library includes
#include <stdlib.h>
#include <string.h>

void oled_init()
{
    Uint8 init_cmds[20] =
    {
         OLED_CMD_SET_DISP_START_LINE,
         OLED_CMD_NORM_SEG_MAP,
         OLED_CMD_SCAN_0_TO_N,
         OLED_CMD_SET_DISP_CONTRAST, 0x80,
         OLED_CMD_SET_MULTIPLEX_RATION, 0x3F,
         OLED_CMD_SET_DC_DC_CONTROL_MOD, 0x81,
         OLED_CMD_SET_OSCILLATOR_FREQ, 0x50,
         OLED_CMD_SET_DISP_OFFSET_MOD, 0x00,
         OLED_CMD_SET_PRE_CHARGE_PERIOD, 0x22,
         OLED_CMD_SET_VCOM, 0x35,
         OLED_CMD_SET_VSEG, 0x35,
         OLED_CMD_SET_DISCHARGE_LEVEL
    };

    EALLOW;
    //RST (reset) - GPIO59 (pin 14)
    GpioCtrlRegs.GPBMUX2.bit.GPIO59 = 0; //set for use as gpio
    GpioCtrlRegs.GPBDIR.bit.GPIO59 = 1U; //set as output
    GpioDataRegs.GPBSET.bit.GPIO59 = 1; //set RST as initially high

    //DC (data/command) - GPIO124 (pin 13)
    GpioCtrlRegs.GPDMUX2.bit.GPIO124 = 0; //set for use as gpio
    GpioCtrlRegs.GPDDIR.bit.GPIO124 = 1U; //set as output
    GpioDataRegs.GPDSET.bit.GPIO124 = 1; //set DC as initially high

    EDIS;

    oled_reset(); //reset display
    oled_cmd_power_off(); //power off
    oled_send_cmd(init_cmds, 20);
    oled_cmd_power_on(); //power on
    oled_clear_RAM();
}


void oled_send_cmd(Uint8 *cmd, Uint8 length)
{
    OLED_ASSRT_CMD;
    spi_transmit(cmd, length);
}

void oled_write_data(Uint8 *data, uint32_t length)
{
    OLED_ASSRT_DATA;
    spi_transmit(data, length);
}

void oled_reset()
{
    OLED_ASSRT_RST;
    Task_sleep(10U);
    OLED_DE_ASSRT_RST;
    Task_sleep(10U);
}

void oled_clear_RAM()
{
    oled_clear_buffer();
    oled_send_buffer();
}

void oled_cmd_power_on()
{
    Uint8 cmd = OLED_CMD_POWER_ON;
    oled_send_cmd(&cmd, 1);
}

void oled_cmd_power_off()
{
    Uint8 cmd = OLED_CMD_POWER_OFF;
    oled_send_cmd(&cmd, 1);
}

void oled_send_buffer()
{

    for (uint32_t i = 0; i < OLED_HEIGHT; i++)
    {
        for (uint32_t j = 0; j < OLED_WIDTH - 1; j = j + 2)
        {
            send_buffer[(j + i * OLED_WIDTH) / 2] = (frame_buffer[j][i] & 0xF0) | ((frame_buffer[j + 1][i]) & 0x0F);
        }
    }


    oled_write_data(send_buffer, OLED_WIDTH * OLED_HEIGHT / 2);

}

void oled_clear_buffer()
{
    memset(frame_buffer, 0, sizeof(frame_buffer));
}


void oled_set_pixel(uint16_t x, uint16_t y, intensity_t intensity)
{
    if((x <= OLED_WIDTH) && (y <= OLED_HEIGHT))
    {
        if(frame_buffer[x][y] < intensity)
            frame_buffer[x][y] = intensity;
    }
}

void oled_draw_line(int16_t x_1, int16_t y_1, int16_t x_2, int16_t y_2, intensity_t intensity)
{
    const int16_t delta_x = abs(x_2 - x_1);
    const int16_t delta_y = abs(y_2 - y_1);
    const int16_t sign_x = x_1 < x_2 ? 1 : -1;
    const int16_t sign_y = y_1 < y_2 ? 1 : -1;
    int16_t error = delta_x - delta_y;

    oled_set_pixel(x_2, y_2, intensity);

    while (x_1 != x_2 || y_1 != y_2)
    {
        oled_set_pixel(x_1, y_1, intensity);

        const int16_t error_2 = error * 2;

        if (error_2 > -delta_y)
        {
            error -= delta_y;
            x_1 += sign_x;
        }
        if (error_2 < delta_x)
        {
            error += delta_x;
            y_1 += sign_y;
        }
    }
}

void oled_draw_frame(int16_t x_1, int16_t y_1, int16_t width, int16_t height, intensity_t intensity)
{

    oled_draw_line(x_1, y_1, x_1, (y_1 + height - 1), intensity);
    oled_draw_line((x_1 + width - 1), y_1, (x_1 + width -  1), (y_1 + height - 1), intensity);
    oled_draw_line(x_1, y_1, (x_1 + width - 1), y_1, intensity);
    oled_draw_line(x_1, (y_1 + height - 1), (x_1 + width - 1), (y_1 + height - 1), intensity);
}

void oled_draw_rectangle(int16_t x_1, int16_t y_1, int16_t width, int16_t height, intensity_t intensity)
{
    for(uint16_t j = 0; j < height; j++)
    {
        for(uint16_t i = 0; i < width; i++)
            oled_set_pixel((x_1 + i), (y_1 + j), intensity);

    }

}

void oled_draw_progres_bar(int16_t x, int16_t y, int16_t height,int16_t bar_width_max, int16_t bar_width_min, int16_t data_max, int16_t data_min, int16_t data)
{
    uint16_t bar_width = floor((float)(data - data_min) * (float)(bar_width_max - bar_width_min) / (float)(data_max - data_min) + (float)bar_width_min);
    oled_draw_frame(x, y, bar_width_max, height, oled_intens_13);
    oled_draw_rectangle(x, y, bar_width, height, oled_intens_5);
}

void oled_draw_char(int16_t x_1, int16_t y_1, char_map_t *map, intensity_t intensity)
{
    uint16_t i = 0;
    uint16_t row = 0;
    uint16_t col = 0;



    for(i = 0; i < (2 * map->height); i += 2) //2 chars per row always with how fonts have been defined
    {
        row = (uint16_t)((map->bit_map[i] << 8) & 0xFF00) | (uint16_t)(map->bit_map[i + 1] & 0x00FF); //combine chars to make row

        for(col = 0; col < map->width; col++) //iterate through and clear/draw pixels
        {
            if((row & (1U << (15U - col)))) //if pixel is high
            {
                oled_set_pixel((x_1 + col), (y_1 + (i/2)), intensity);
            }
        }

    }
}

void oled_draw_string(int16_t x_1, int16_t y_1, intensity_t intensity, font_def_t font, const char *format, ...)
{
    char *buffer;
    uint16_t length;
    va_list args;
    uint16_t i = 0;
    Uint8 cursor = 0;
    char_map_t *map;

    va_start(args, format); //initialize va_list (used to dynamically parse arguments)

    length = vsnprintf(NULL, 0, format, args);     //calculate the length required for the formatted string
    buffer = (char *)malloc(length + 1);           //allocate a buffer for formatted string


    vsnprintf(buffer, length + 1, format, args); //write string to buffer

    cursor = x_1;

    for(i = 0; i < length; i++)
    {
        map = decode_char(buffer[i], font);
        oled_draw_char(cursor, y_1, map, intensity);
        cursor += map->width;
    }

    //cleanup
    free(buffer); //de-allocate buffer
    va_end(args); //cleanup va_list
}


void oled_set_orientation(bool flipped)
{
    //rotate orientation 180 degrees
    if(flipped)
    {
        //oled_cmd_set_row_addr(32);
        //oled_cmd_set_scan_dir(false);
        //oled_cmd_set_segment_remap(true);
    }
}

