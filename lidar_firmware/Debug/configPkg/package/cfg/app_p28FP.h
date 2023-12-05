/*
 *  Do not modify this file; it is automatically 
 *  generated and any modifications will be overwritten.
 *
 * @(#) xdc-K04
 */

#include <xdc/std.h>

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle lidar_sample_tsk_hdl;

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle step_tsk_hdl;

#include <ti/sysbios/family/c28/Hwi.h>
extern const ti_sysbios_family_c28_Hwi_Handle hwi0;

#include <ti/sysbios/family/c28/Hwi.h>
extern const ti_sysbios_family_c28_Hwi_Handle hwi1;

#include <ti/sysbios/knl/Semaphore.h>
extern const ti_sysbios_knl_Semaphore_Handle i2c_tx_sem;

#include <ti/sysbios/knl/Semaphore.h>
extern const ti_sysbios_knl_Semaphore_Handle i2c_rx_sem;

#include <ti/sysbios/knl/Semaphore.h>
extern const ti_sysbios_knl_Semaphore_Handle spi_tx_sem;

#include <ti/sysbios/family/c28/Hwi.h>
extern const ti_sysbios_family_c28_Hwi_Handle hwi2;

#include <ti/sysbios/knl/Semaphore.h>
extern const ti_sysbios_knl_Semaphore_Handle uart_tx_sem;

#include <ti/sysbios/family/c28/Hwi.h>
extern const ti_sysbios_family_c28_Hwi_Handle hwi3;

#include <ti/sysbios/family/c28/Hwi.h>
extern const ti_sysbios_family_c28_Hwi_Handle hwi4;

#include <ti/sysbios/knl/Swi.h>
extern const ti_sysbios_knl_Swi_Handle uart_rx_swi_hdl;

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle oled_display_tsk_hdl;

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle pc_data_tx_tsk_hdl;

#include <ti/sysbios/knl/Semaphore.h>
extern const ti_sysbios_knl_Semaphore_Handle step_to_sample_sem;

#include <ti/sysbios/knl/Semaphore.h>
extern const ti_sysbios_knl_Semaphore_Handle sample_to_step_sem;

#include <ti/sysbios/knl/Semaphore.h>
extern const ti_sysbios_knl_Semaphore_Handle step_delay_sem;

#include <ti/sysbios/hal/Timer.h>
extern const ti_sysbios_hal_Timer_Handle delay_timer_hdl;

#include <ti/sysbios/knl/Semaphore.h>
extern const ti_sysbios_knl_Semaphore_Handle lidar_sample_delay_sem;

#include <ti/sysbios/knl/Semaphore.h>
extern const ti_sysbios_knl_Semaphore_Handle zero_sem;

#include <ti/sysbios/knl/Swi.h>
extern const ti_sysbios_knl_Swi_Handle i2c_tx_swi_hdl;

#include <ti/sysbios/knl/Swi.h>
extern const ti_sysbios_knl_Swi_Handle i2c_rx_swi_hdl;

extern int xdc_runtime_Startup__EXECFXN__C;

extern int xdc_runtime_Startup__RESETFXN__C;

extern int xdc_rov_runtime_Mon__checksum;

extern int xdc_rov_runtime_Mon__write;

