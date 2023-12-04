/***********************************************************************************
 * stepper.h
 *
 *
 *
 *
 * Author:    Omar Kebedov
 * Date:      12/01/2023
 * Modified:  12/01/2023
 *********************************************************************************/
#ifndef STEPPER_H_
#define STEPPER_H_

#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Types.h>
#include <ti/sysbios/BIOS.h>
#include <Headers/F2837xD_device.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>

void stepper_init(); //initialize GPIO for stepper, on;y three pins need to be used

void stepper_step(uint16_t steps); //make a step

void ir_sensor_ISR(void);

#endif //ifndef
