################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Add inputs and outputs from these tool invocations to the build variables 
CFG_SRCS += \
../app.cfg 

CMD_SRCS += \
../F2837xD_Headers_BIOS_cpu1.cmd \
../TMS320F28379D.cmd 

C_SRCS += \
../F2837xD_GlobalVariableDefs.c \
../delay_timer.c \
../main.c 

GEN_CMDS += \
./configPkg/linker.cmd 

GEN_FILES += \
./configPkg/linker.cmd \
./configPkg/compiler.opt 

GEN_MISC_DIRS += \
./configPkg/ 

C_DEPS += \
./F2837xD_GlobalVariableDefs.d \
./delay_timer.d \
./main.d 

GEN_OPTS += \
./configPkg/compiler.opt 

OBJS += \
./F2837xD_GlobalVariableDefs.obj \
./delay_timer.obj \
./main.obj 

GEN_MISC_DIRS__QUOTED += \
"configPkg\" 

OBJS__QUOTED += \
"F2837xD_GlobalVariableDefs.obj" \
"delay_timer.obj" \
"main.obj" 

C_DEPS__QUOTED += \
"F2837xD_GlobalVariableDefs.d" \
"delay_timer.d" \
"main.d" 

GEN_FILES__QUOTED += \
"configPkg\linker.cmd" \
"configPkg\compiler.opt" 

C_SRCS__QUOTED += \
"../F2837xD_GlobalVariableDefs.c" \
"../delay_timer.c" \
"../main.c" 


