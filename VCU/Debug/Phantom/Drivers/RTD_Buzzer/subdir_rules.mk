################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
Phantom/Drivers/RTD_Buzzer/%.obj: ../Phantom/Drivers/RTD_Buzzer/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccs930/ccs/tools/compiler/ti-cgt-arm_18.12.4.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 --include_path="C:/Users/Micha/OneDrive/Documents/SFU/Phantom/NEW_VCU_RX/vcu-fw/VCU" --include_path="C:/Users/Micha/OneDrive/Documents/SFU/Phantom/NEW_VCU_RX/vcu-fw/VCU/Phantom/hardware/vcu_hw" --include_path="C:/ti/ccs930/ccs/tools/compiler/ti-cgt-arm_18.12.4.LTS/include" --include_path="C:/Users/Micha/OneDrive/Documents/SFU/Phantom/NEW_VCU_RX/vcu-fw/VCU/Phantom/tasks" --include_path="C:/Users/Micha/OneDrive/Documents/SFU/Phantom/NEW_VCU_RX/vcu-fw/VCU/Phantom/hardware" --include_path="C:/Users/Micha/OneDrive/Documents/SFU/Phantom/NEW_VCU_RX/vcu-fw/VCU/Phantom/Drivers/UART" --include_path="C:/Users/Micha/OneDrive/Documents/SFU/Phantom/NEW_VCU_RX/vcu-fw/VCU/Phantom/Drivers/DAC_SPI" --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="Phantom/Drivers/RTD_Buzzer/$(basename $(<F)).d_raw" --obj_directory="Phantom/Drivers/RTD_Buzzer" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


