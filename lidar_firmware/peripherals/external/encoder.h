#ifndef ENCODER_H_
#define ENCODER_H_

#include <Headers/F2837xD_device.h>
#include <ti/sysbios/knl/Swi.h>

void encoder_init();
extern const Swi_Handle encoder_swi_hdl; //SWI handle associated with SWI used for servicing UART receive events

void encoder_SWI(void);
void encoder_ch_a_ISR(void);
void encoder_switch_ISR(void);

#endif /*ENCODER_H_ */
