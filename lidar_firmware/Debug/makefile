################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccs1040/ccs/tools/compiler/ti-cgt-c2000_20.2.5.LTS

GEN_OPTS__FLAG := --cmd_file="configPkg/compiler.opt" 
GEN_CMDS__FLAG := -l"configPkg/linker.cmd" 

ORDERED_OBJS += \
"./F2837xD_GlobalVariableDefs.obj" \
"./delay_timer.obj" \
"./main.obj" \
"./peripherals/external/oled/SH1122_fonts.obj" \
"./peripherals/external/oled/SH1122_oled.obj" \
"./peripherals/external/oled/fonts/SH1122_font_f10x16f.obj" \
"./peripherals/external/oled/fonts/SH1122_font_f6x8_mono.obj" \
"./peripherals/external/stepper.obj" \
"./peripherals/external/tf_mini_s_lidar.obj" \
"./peripherals/internal/28379D_i2c.obj" \
"./peripherals/internal/28379D_spi.obj" \
"./peripherals/internal/28379D_uart.obj" \
"./peripherals/internal/gpio.obj" \
"../F2837xD_Headers_BIOS_cpu1.cmd" \
"../TMS320F28379D.cmd" \
$(GEN_CMDS__FLAG) \
-llibc.a \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include peripherals/external/oled/subdir_vars.mk
-include peripherals/external/oled/fonts/subdir_vars.mk
-include peripherals/external/subdir_vars.mk
-include peripherals/internal/subdir_vars.mk
-include subdir_rules.mk
-include peripherals/external/oled/subdir_rules.mk
-include peripherals/external/oled/fonts/subdir_rules.mk
-include peripherals/external/subdir_rules.mk
-include peripherals/internal/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
lidar_firmware_v2.out \

EXE_OUTPUTS__QUOTED += \
"lidar_firmware_v2.out" \

BIN_OUTPUTS += \
lidar_firmware_v2.hex \

BIN_OUTPUTS__QUOTED += \
"lidar_firmware_v2.hex" \


# All Target
all: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "lidar_firmware_v2.out"

# Tool invocations
lidar_firmware_v2.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: C2000 Linker'
	"C:/ti/ccs1040/ccs/tools/compiler/ti-cgt-c2000_20.2.5.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --advice:performance=all --define=CPU1 -g --c11 --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=coffabi -z -m"lidar_firmware_v2.map" --stack_size=0x200 --warn_sections -i"C:/ti/ccs1040/ccs/tools/compiler/ti-cgt-c2000_20.2.5.LTS/lib" -i"C:/ti/ccs1040/ccs/tools/compiler/ti-cgt-c2000_20.2.5.LTS/include" --reread_libs --diag_wrap=off --display_error_number --xml_link_info="lidar_firmware_v2_linkInfo.xml" --rom_model -o "lidar_firmware_v2.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '

lidar_firmware_v2.hex: $(EXE_OUTPUTS)
	@echo 'Building secondary target: "$@"'
	@echo 'Invoking: C2000 Hex Utility'
	"C:/ti/ccs1040/ccs/tools/compiler/ti-cgt-c2000_20.2.5.LTS/bin/hex2000" --diag_wrap=off -o "lidar_firmware_v2.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building secondary target: "$@"'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(BIN_OUTPUTS__QUOTED)$(GEN_FILES__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "F2837xD_GlobalVariableDefs.obj" "delay_timer.obj" "main.obj" "peripherals\external\oled\SH1122_fonts.obj" "peripherals\external\oled\SH1122_oled.obj" "peripherals\external\oled\fonts\SH1122_font_f10x16f.obj" "peripherals\external\oled\fonts\SH1122_font_f6x8_mono.obj" "peripherals\external\stepper.obj" "peripherals\external\tf_mini_s_lidar.obj" "peripherals\internal\28379D_i2c.obj" "peripherals\internal\28379D_spi.obj" "peripherals\internal\28379D_uart.obj" "peripherals\internal\gpio.obj" 
	-$(RM) "F2837xD_GlobalVariableDefs.d" "delay_timer.d" "main.d" "peripherals\external\oled\SH1122_fonts.d" "peripherals\external\oled\SH1122_oled.d" "peripherals\external\oled\fonts\SH1122_font_f10x16f.d" "peripherals\external\oled\fonts\SH1122_font_f6x8_mono.d" "peripherals\external\stepper.d" "peripherals\external\tf_mini_s_lidar.d" "peripherals\internal\28379D_i2c.d" "peripherals\internal\28379D_spi.d" "peripherals\internal\28379D_uart.d" "peripherals\internal\gpio.d" 
	-$(RMDIR) $(GEN_MISC_DIRS__QUOTED)
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

