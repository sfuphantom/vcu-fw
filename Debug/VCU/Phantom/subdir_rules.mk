################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
VCU/Phantom/main.obj: ../VCU/Phantom/main.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs1020/ccs/tools/compiler/ti-cgt-arm_20.2.2.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 --include_path="C:/Users/ryanh/Desktop/vcu-fw/VCU" --include_path="C:/Users/ryanh/Desktop/vcu-fw" --include_path="C:/ti/ccs1020/ccs/tools/compiler/ti-cgt-arm_20.2.2.LTS/include" --include_path="C:/Users/ryanh/Desktop/vcu-fw/VCU/halcogen_vcu/source" --include_path="C:/Users/ryanh/Desktop/vcu-fw/VCU/Phantom/Drivers/HV_Voltage_SPI" --include_path="C:/Users/ryanh/Desktop/vcu-fw/VCU/halcogen_vcu/include" -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="VCU/Phantom/$(basename $(<F)).d_raw" --obj_directory="VCU/Phantom" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


