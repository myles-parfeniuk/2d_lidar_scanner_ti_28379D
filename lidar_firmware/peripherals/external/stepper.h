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

#include <math.h>
#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Types.h>
#include <ti/sysbios/BIOS.h>
#include <Headers/F2837xD_device.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Semaphore.h>
#include "../delay_timer.h"

extern const Semaphore_Handle step_delay_sem, zero_sem; //Semaphore used for delay between STEP pin toggles
static float run_window[5] = {8350.0, 8350.0, 8350.0, 8350.0, 8350.0};
static float run_average_steps = 8350.0;
static volatile uint16_t num_steps, num_runs, initialized;
void stepper_init(); //initialize GPIO for stepper, on;y three pins need to be used
void stepper_zero(); //zero the stepper at the beginning of the execution so we know current position
uint16_t stepper_step(uint16_t steps); //make a step

void ir_sensor_ISR(void);

#endif //ifndef
