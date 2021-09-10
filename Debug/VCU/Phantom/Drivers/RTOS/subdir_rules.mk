################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
VCU/Phantom/Drivers/RTOS/%.obj: ../VCU/Phantom/Drivers/RTOS/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs1020/ccs/tools/compiler/ti-cgt-arm_20.2.2.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 --include_path="C:/Users/ryanh/Desktop/vcu-fw/VCU/Phantom/tasks/headers" --include_path="C:/Users/ryanh/Desktop/vcu-fw/VCU/Phantom" --include_path="C:/ti/Hercules/F021 Flash API/02.01.01/include" --include_path="C:/ti/Hercules/F021 Flash API/02.01.01/source" --include_path="C:/Users/ryanh/Desktop/vcu-fw/VCU/Phantom/data_structures" --include_path="C:/Users/ryanh/Desktop/vcu-fw/VCU/Phantom/tasks/source" --include_path="C:/Users/ryanh/Desktop/vcu-fw/VCU/halcogen_vcu" --include_path="C:/Users/ryanh/Desktop/vcu-fw/VCU/Phantom/Drivers/DAC_SPI" --include_path="C:/Users/ryanh/Desktop/vcu-fw/VCU/Phantom/Drivers/EEPROM" --include_path="C:/Users/ryanh/Desktop/vcu-fw/VCU/Phantom/Drivers/UART" --include_path="C:/Users/ryanh/Desktop/vcu-fw/VCU/Phantom/tasks/source" --include_path="C:/Users/ryanh/Desktop/vcu-fw/VCU/Phantom/Drivers/RTOS" --include_path="C:/Users/ryanh/Desktop/vcu-fw/VCU/Phantom/Drivers/HV_Voltage_SPI" --include_path="C:/Users/ryanh/Desktop/vcu-fw/VCU/halcogen_vcu/include" --include_path="C:/Users/ryanh/Desktop/vcu-fw/VCU/halcogen_vcu/source" --include_path="C:/Users/ryanh/Desktop/vcu-fw/VCU/Phantom/hardware/vcu_hw" --include_path="C:/Users/ryanh/Desktop/vcu-fw" --include_path="C:/ti/ccs1020/ccs/tools/compiler/ti-cgt-arm_20.2.2.LTS/include" -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="VCU/Phantom/Drivers/RTOS/$(basename $(<F)).d_raw" --obj_directory="VCU/Phantom/Drivers/RTOS" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


