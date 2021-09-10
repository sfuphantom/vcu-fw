################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Add inputs and outputs from these tool invocations to the build variables 
CMD_SRCS += \
../VCU/halcogen_launchpad/source/sys_link.cmd 

ASM_SRCS += \
../VCU/halcogen_launchpad/source/dabort.asm \
../VCU/halcogen_launchpad/source/os_portasm.asm \
../VCU/halcogen_launchpad/source/svc.asm \
../VCU/halcogen_launchpad/source/sys_core.asm \
../VCU/halcogen_launchpad/source/sys_intvecs.asm \
../VCU/halcogen_launchpad/source/sys_mpu.asm \
../VCU/halcogen_launchpad/source/sys_pmu.asm 

C_SRCS += \
../VCU/halcogen_launchpad/source/Device_TMS570LS12.c \
../VCU/halcogen_launchpad/source/Fapi_UserDefinedFunctions.c \
../VCU/halcogen_launchpad/source/adc.c \
../VCU/halcogen_launchpad/source/errata_SSWF021_45.c \
../VCU/halcogen_launchpad/source/esm.c \
../VCU/halcogen_launchpad/source/gio.c \
../VCU/halcogen_launchpad/source/het.c \
../VCU/halcogen_launchpad/source/i2c.c \
../VCU/halcogen_launchpad/source/mibspi.c \
../VCU/halcogen_launchpad/source/notification.c \
../VCU/halcogen_launchpad/source/os_croutine.c \
../VCU/halcogen_launchpad/source/os_event_groups.c \
../VCU/halcogen_launchpad/source/os_heap.c \
../VCU/halcogen_launchpad/source/os_list.c \
../VCU/halcogen_launchpad/source/os_mpu_wrappers.c \
../VCU/halcogen_launchpad/source/os_port.c \
../VCU/halcogen_launchpad/source/os_queue.c \
../VCU/halcogen_launchpad/source/os_tasks.c \
../VCU/halcogen_launchpad/source/os_timer.c \
../VCU/halcogen_launchpad/source/pinmux.c \
../VCU/halcogen_launchpad/source/rti.c \
../VCU/halcogen_launchpad/source/sci.c \
../VCU/halcogen_launchpad/source/sys_dma.c \
../VCU/halcogen_launchpad/source/sys_main.c \
../VCU/halcogen_launchpad/source/sys_pcr.c \
../VCU/halcogen_launchpad/source/sys_phantom.c \
../VCU/halcogen_launchpad/source/sys_pmm.c \
../VCU/halcogen_launchpad/source/sys_selftest.c \
../VCU/halcogen_launchpad/source/sys_startup.c \
../VCU/halcogen_launchpad/source/sys_vim.c \
../VCU/halcogen_launchpad/source/system.c \
../VCU/halcogen_launchpad/source/ti_fee_Info.c \
../VCU/halcogen_launchpad/source/ti_fee_cancel.c \
../VCU/halcogen_launchpad/source/ti_fee_cfg.c \
../VCU/halcogen_launchpad/source/ti_fee_eraseimmediateblock.c \
../VCU/halcogen_launchpad/source/ti_fee_format.c \
../VCU/halcogen_launchpad/source/ti_fee_ini.c \
../VCU/halcogen_launchpad/source/ti_fee_invalidateblock.c \
../VCU/halcogen_launchpad/source/ti_fee_main.c \
../VCU/halcogen_launchpad/source/ti_fee_read.c \
../VCU/halcogen_launchpad/source/ti_fee_readSync.c \
../VCU/halcogen_launchpad/source/ti_fee_shutdown.c \
../VCU/halcogen_launchpad/source/ti_fee_util.c \
../VCU/halcogen_launchpad/source/ti_fee_writeAsync.c \
../VCU/halcogen_launchpad/source/ti_fee_writeSync.c 

C_DEPS += \
./VCU/halcogen_launchpad/source/Device_TMS570LS12.d \
./VCU/halcogen_launchpad/source/Fapi_UserDefinedFunctions.d \
./VCU/halcogen_launchpad/source/adc.d \
./VCU/halcogen_launchpad/source/errata_SSWF021_45.d \
./VCU/halcogen_launchpad/source/esm.d \
./VCU/halcogen_launchpad/source/gio.d \
./VCU/halcogen_launchpad/source/het.d \
./VCU/halcogen_launchpad/source/i2c.d \
./VCU/halcogen_launchpad/source/mibspi.d \
./VCU/halcogen_launchpad/source/notification.d \
./VCU/halcogen_launchpad/source/os_croutine.d \
./VCU/halcogen_launchpad/source/os_event_groups.d \
./VCU/halcogen_launchpad/source/os_heap.d \
./VCU/halcogen_launchpad/source/os_list.d \
./VCU/halcogen_launchpad/source/os_mpu_wrappers.d \
./VCU/halcogen_launchpad/source/os_port.d \
./VCU/halcogen_launchpad/source/os_queue.d \
./VCU/halcogen_launchpad/source/os_tasks.d \
./VCU/halcogen_launchpad/source/os_timer.d \
./VCU/halcogen_launchpad/source/pinmux.d \
./VCU/halcogen_launchpad/source/rti.d \
./VCU/halcogen_launchpad/source/sci.d \
./VCU/halcogen_launchpad/source/sys_dma.d \
./VCU/halcogen_launchpad/source/sys_main.d \
./VCU/halcogen_launchpad/source/sys_pcr.d \
./VCU/halcogen_launchpad/source/sys_phantom.d \
./VCU/halcogen_launchpad/source/sys_pmm.d \
./VCU/halcogen_launchpad/source/sys_selftest.d \
./VCU/halcogen_launchpad/source/sys_startup.d \
./VCU/halcogen_launchpad/source/sys_vim.d \
./VCU/halcogen_launchpad/source/system.d \
./VCU/halcogen_launchpad/source/ti_fee_Info.d \
./VCU/halcogen_launchpad/source/ti_fee_cancel.d \
./VCU/halcogen_launchpad/source/ti_fee_cfg.d \
./VCU/halcogen_launchpad/source/ti_fee_eraseimmediateblock.d \
./VCU/halcogen_launchpad/source/ti_fee_format.d \
./VCU/halcogen_launchpad/source/ti_fee_ini.d \
./VCU/halcogen_launchpad/source/ti_fee_invalidateblock.d \
./VCU/halcogen_launchpad/source/ti_fee_main.d \
./VCU/halcogen_launchpad/source/ti_fee_read.d \
./VCU/halcogen_launchpad/source/ti_fee_readSync.d \
./VCU/halcogen_launchpad/source/ti_fee_shutdown.d \
./VCU/halcogen_launchpad/source/ti_fee_util.d \
./VCU/halcogen_launchpad/source/ti_fee_writeAsync.d \
./VCU/halcogen_launchpad/source/ti_fee_writeSync.d 

OBJS += \
./VCU/halcogen_launchpad/source/Device_TMS570LS12.obj \
./VCU/halcogen_launchpad/source/Fapi_UserDefinedFunctions.obj \
./VCU/halcogen_launchpad/source/adc.obj \
./VCU/halcogen_launchpad/source/dabort.obj \
./VCU/halcogen_launchpad/source/errata_SSWF021_45.obj \
./VCU/halcogen_launchpad/source/esm.obj \
./VCU/halcogen_launchpad/source/gio.obj \
./VCU/halcogen_launchpad/source/het.obj \
./VCU/halcogen_launchpad/source/i2c.obj \
./VCU/halcogen_launchpad/source/mibspi.obj \
./VCU/halcogen_launchpad/source/notification.obj \
./VCU/halcogen_launchpad/source/os_croutine.obj \
./VCU/halcogen_launchpad/source/os_event_groups.obj \
./VCU/halcogen_launchpad/source/os_heap.obj \
./VCU/halcogen_launchpad/source/os_list.obj \
./VCU/halcogen_launchpad/source/os_mpu_wrappers.obj \
./VCU/halcogen_launchpad/source/os_port.obj \
./VCU/halcogen_launchpad/source/os_portasm.obj \
./VCU/halcogen_launchpad/source/os_queue.obj \
./VCU/halcogen_launchpad/source/os_tasks.obj \
./VCU/halcogen_launchpad/source/os_timer.obj \
./VCU/halcogen_launchpad/source/pinmux.obj \
./VCU/halcogen_launchpad/source/rti.obj \
./VCU/halcogen_launchpad/source/sci.obj \
./VCU/halcogen_launchpad/source/svc.obj \
./VCU/halcogen_launchpad/source/sys_core.obj \
./VCU/halcogen_launchpad/source/sys_dma.obj \
./VCU/halcogen_launchpad/source/sys_intvecs.obj \
./VCU/halcogen_launchpad/source/sys_main.obj \
./VCU/halcogen_launchpad/source/sys_mpu.obj \
./VCU/halcogen_launchpad/source/sys_pcr.obj \
./VCU/halcogen_launchpad/source/sys_phantom.obj \
./VCU/halcogen_launchpad/source/sys_pmm.obj \
./VCU/halcogen_launchpad/source/sys_pmu.obj \
./VCU/halcogen_launchpad/source/sys_selftest.obj \
./VCU/halcogen_launchpad/source/sys_startup.obj \
./VCU/halcogen_launchpad/source/sys_vim.obj \
./VCU/halcogen_launchpad/source/system.obj \
./VCU/halcogen_launchpad/source/ti_fee_Info.obj \
./VCU/halcogen_launchpad/source/ti_fee_cancel.obj \
./VCU/halcogen_launchpad/source/ti_fee_cfg.obj \
./VCU/halcogen_launchpad/source/ti_fee_eraseimmediateblock.obj \
./VCU/halcogen_launchpad/source/ti_fee_format.obj \
./VCU/halcogen_launchpad/source/ti_fee_ini.obj \
./VCU/halcogen_launchpad/source/ti_fee_invalidateblock.obj \
./VCU/halcogen_launchpad/source/ti_fee_main.obj \
./VCU/halcogen_launchpad/source/ti_fee_read.obj \
./VCU/halcogen_launchpad/source/ti_fee_readSync.obj \
./VCU/halcogen_launchpad/source/ti_fee_shutdown.obj \
./VCU/halcogen_launchpad/source/ti_fee_util.obj \
./VCU/halcogen_launchpad/source/ti_fee_writeAsync.obj \
./VCU/halcogen_launchpad/source/ti_fee_writeSync.obj 

ASM_DEPS += \
./VCU/halcogen_launchpad/source/dabort.d \
./VCU/halcogen_launchpad/source/os_portasm.d \
./VCU/halcogen_launchpad/source/svc.d \
./VCU/halcogen_launchpad/source/sys_core.d \
./VCU/halcogen_launchpad/source/sys_intvecs.d \
./VCU/halcogen_launchpad/source/sys_mpu.d \
./VCU/halcogen_launchpad/source/sys_pmu.d 

OBJS__QUOTED += \
"VCU\halcogen_launchpad\source\Device_TMS570LS12.obj" \
"VCU\halcogen_launchpad\source\Fapi_UserDefinedFunctions.obj" \
"VCU\halcogen_launchpad\source\adc.obj" \
"VCU\halcogen_launchpad\source\dabort.obj" \
"VCU\halcogen_launchpad\source\errata_SSWF021_45.obj" \
"VCU\halcogen_launchpad\source\esm.obj" \
"VCU\halcogen_launchpad\source\gio.obj" \
"VCU\halcogen_launchpad\source\het.obj" \
"VCU\halcogen_launchpad\source\i2c.obj" \
"VCU\halcogen_launchpad\source\mibspi.obj" \
"VCU\halcogen_launchpad\source\notification.obj" \
"VCU\halcogen_launchpad\source\os_croutine.obj" \
"VCU\halcogen_launchpad\source\os_event_groups.obj" \
"VCU\halcogen_launchpad\source\os_heap.obj" \
"VCU\halcogen_launchpad\source\os_list.obj" \
"VCU\halcogen_launchpad\source\os_mpu_wrappers.obj" \
"VCU\halcogen_launchpad\source\os_port.obj" \
"VCU\halcogen_launchpad\source\os_portasm.obj" \
"VCU\halcogen_launchpad\source\os_queue.obj" \
"VCU\halcogen_launchpad\source\os_tasks.obj" \
"VCU\halcogen_launchpad\source\os_timer.obj" \
"VCU\halcogen_launchpad\source\pinmux.obj" \
"VCU\halcogen_launchpad\source\rti.obj" \
"VCU\halcogen_launchpad\source\sci.obj" \
"VCU\halcogen_launchpad\source\svc.obj" \
"VCU\halcogen_launchpad\source\sys_core.obj" \
"VCU\halcogen_launchpad\source\sys_dma.obj" \
"VCU\halcogen_launchpad\source\sys_intvecs.obj" \
"VCU\halcogen_launchpad\source\sys_main.obj" \
"VCU\halcogen_launchpad\source\sys_mpu.obj" \
"VCU\halcogen_launchpad\source\sys_pcr.obj" \
"VCU\halcogen_launchpad\source\sys_phantom.obj" \
"VCU\halcogen_launchpad\source\sys_pmm.obj" \
"VCU\halcogen_launchpad\source\sys_pmu.obj" \
"VCU\halcogen_launchpad\source\sys_selftest.obj" \
"VCU\halcogen_launchpad\source\sys_startup.obj" \
"VCU\halcogen_launchpad\source\sys_vim.obj" \
"VCU\halcogen_launchpad\source\system.obj" \
"VCU\halcogen_launchpad\source\ti_fee_Info.obj" \
"VCU\halcogen_launchpad\source\ti_fee_cancel.obj" \
"VCU\halcogen_launchpad\source\ti_fee_cfg.obj" \
"VCU\halcogen_launchpad\source\ti_fee_eraseimmediateblock.obj" \
"VCU\halcogen_launchpad\source\ti_fee_format.obj" \
"VCU\halcogen_launchpad\source\ti_fee_ini.obj" \
"VCU\halcogen_launchpad\source\ti_fee_invalidateblock.obj" \
"VCU\halcogen_launchpad\source\ti_fee_main.obj" \
"VCU\halcogen_launchpad\source\ti_fee_read.obj" \
"VCU\halcogen_launchpad\source\ti_fee_readSync.obj" \
"VCU\halcogen_launchpad\source\ti_fee_shutdown.obj" \
"VCU\halcogen_launchpad\source\ti_fee_util.obj" \
"VCU\halcogen_launchpad\source\ti_fee_writeAsync.obj" \
"VCU\halcogen_launchpad\source\ti_fee_writeSync.obj" 

C_DEPS__QUOTED += \
"VCU\halcogen_launchpad\source\Device_TMS570LS12.d" \
"VCU\halcogen_launchpad\source\Fapi_UserDefinedFunctions.d" \
"VCU\halcogen_launchpad\source\adc.d" \
"VCU\halcogen_launchpad\source\errata_SSWF021_45.d" \
"VCU\halcogen_launchpad\source\esm.d" \
"VCU\halcogen_launchpad\source\gio.d" \
"VCU\halcogen_launchpad\source\het.d" \
"VCU\halcogen_launchpad\source\i2c.d" \
"VCU\halcogen_launchpad\source\mibspi.d" \
"VCU\halcogen_launchpad\source\notification.d" \
"VCU\halcogen_launchpad\source\os_croutine.d" \
"VCU\halcogen_launchpad\source\os_event_groups.d" \
"VCU\halcogen_launchpad\source\os_heap.d" \
"VCU\halcogen_launchpad\source\os_list.d" \
"VCU\halcogen_launchpad\source\os_mpu_wrappers.d" \
"VCU\halcogen_launchpad\source\os_port.d" \
"VCU\halcogen_launchpad\source\os_queue.d" \
"VCU\halcogen_launchpad\source\os_tasks.d" \
"VCU\halcogen_launchpad\source\os_timer.d" \
"VCU\halcogen_launchpad\source\pinmux.d" \
"VCU\halcogen_launchpad\source\rti.d" \
"VCU\halcogen_launchpad\source\sci.d" \
"VCU\halcogen_launchpad\source\sys_dma.d" \
"VCU\halcogen_launchpad\source\sys_main.d" \
"VCU\halcogen_launchpad\source\sys_pcr.d" \
"VCU\halcogen_launchpad\source\sys_phantom.d" \
"VCU\halcogen_launchpad\source\sys_pmm.d" \
"VCU\halcogen_launchpad\source\sys_selftest.d" \
"VCU\halcogen_launchpad\source\sys_startup.d" \
"VCU\halcogen_launchpad\source\sys_vim.d" \
"VCU\halcogen_launchpad\source\system.d" \
"VCU\halcogen_launchpad\source\ti_fee_Info.d" \
"VCU\halcogen_launchpad\source\ti_fee_cancel.d" \
"VCU\halcogen_launchpad\source\ti_fee_cfg.d" \
"VCU\halcogen_launchpad\source\ti_fee_eraseimmediateblock.d" \
"VCU\halcogen_launchpad\source\ti_fee_format.d" \
"VCU\halcogen_launchpad\source\ti_fee_ini.d" \
"VCU\halcogen_launchpad\source\ti_fee_invalidateblock.d" \
"VCU\halcogen_launchpad\source\ti_fee_main.d" \
"VCU\halcogen_launchpad\source\ti_fee_read.d" \
"VCU\halcogen_launchpad\source\ti_fee_readSync.d" \
"VCU\halcogen_launchpad\source\ti_fee_shutdown.d" \
"VCU\halcogen_launchpad\source\ti_fee_util.d" \
"VCU\halcogen_launchpad\source\ti_fee_writeAsync.d" \
"VCU\halcogen_launchpad\source\ti_fee_writeSync.d" 

ASM_DEPS__QUOTED += \
"VCU\halcogen_launchpad\source\dabort.d" \
"VCU\halcogen_launchpad\source\os_portasm.d" \
"VCU\halcogen_launchpad\source\svc.d" \
"VCU\halcogen_launchpad\source\sys_core.d" \
"VCU\halcogen_launchpad\source\sys_intvecs.d" \
"VCU\halcogen_launchpad\source\sys_mpu.d" \
"VCU\halcogen_launchpad\source\sys_pmu.d" 

C_SRCS__QUOTED += \
"../VCU/halcogen_launchpad/source/Device_TMS570LS12.c" \
"../VCU/halcogen_launchpad/source/Fapi_UserDefinedFunctions.c" \
"../VCU/halcogen_launchpad/source/adc.c" \
"../VCU/halcogen_launchpad/source/errata_SSWF021_45.c" \
"../VCU/halcogen_launchpad/source/esm.c" \
"../VCU/halcogen_launchpad/source/gio.c" \
"../VCU/halcogen_launchpad/source/het.c" \
"../VCU/halcogen_launchpad/source/i2c.c" \
"../VCU/halcogen_launchpad/source/mibspi.c" \
"../VCU/halcogen_launchpad/source/notification.c" \
"../VCU/halcogen_launchpad/source/os_croutine.c" \
"../VCU/halcogen_launchpad/source/os_event_groups.c" \
"../VCU/halcogen_launchpad/source/os_heap.c" \
"../VCU/halcogen_launchpad/source/os_list.c" \
"../VCU/halcogen_launchpad/source/os_mpu_wrappers.c" \
"../VCU/halcogen_launchpad/source/os_port.c" \
"../VCU/halcogen_launchpad/source/os_queue.c" \
"../VCU/halcogen_launchpad/source/os_tasks.c" \
"../VCU/halcogen_launchpad/source/os_timer.c" \
"../VCU/halcogen_launchpad/source/pinmux.c" \
"../VCU/halcogen_launchpad/source/rti.c" \
"../VCU/halcogen_launchpad/source/sci.c" \
"../VCU/halcogen_launchpad/source/sys_dma.c" \
"../VCU/halcogen_launchpad/source/sys_main.c" \
"../VCU/halcogen_launchpad/source/sys_pcr.c" \
"../VCU/halcogen_launchpad/source/sys_phantom.c" \
"../VCU/halcogen_launchpad/source/sys_pmm.c" \
"../VCU/halcogen_launchpad/source/sys_selftest.c" \
"../VCU/halcogen_launchpad/source/sys_startup.c" \
"../VCU/halcogen_launchpad/source/sys_vim.c" \
"../VCU/halcogen_launchpad/source/system.c" \
"../VCU/halcogen_launchpad/source/ti_fee_Info.c" \
"../VCU/halcogen_launchpad/source/ti_fee_cancel.c" \
"../VCU/halcogen_launchpad/source/ti_fee_cfg.c" \
"../VCU/halcogen_launchpad/source/ti_fee_eraseimmediateblock.c" \
"../VCU/halcogen_launchpad/source/ti_fee_format.c" \
"../VCU/halcogen_launchpad/source/ti_fee_ini.c" \
"../VCU/halcogen_launchpad/source/ti_fee_invalidateblock.c" \
"../VCU/halcogen_launchpad/source/ti_fee_main.c" \
"../VCU/halcogen_launchpad/source/ti_fee_read.c" \
"../VCU/halcogen_launchpad/source/ti_fee_readSync.c" \
"../VCU/halcogen_launchpad/source/ti_fee_shutdown.c" \
"../VCU/halcogen_launchpad/source/ti_fee_util.c" \
"../VCU/halcogen_launchpad/source/ti_fee_writeAsync.c" \
"../VCU/halcogen_launchpad/source/ti_fee_writeSync.c" 

ASM_SRCS__QUOTED += \
"../VCU/halcogen_launchpad/source/dabort.asm" \
"../VCU/halcogen_launchpad/source/os_portasm.asm" \
"../VCU/halcogen_launchpad/source/svc.asm" \
"../VCU/halcogen_launchpad/source/sys_intvecs.asm" \
"../VCU/halcogen_launchpad/source/sys_mpu.asm" \
"../VCU/halcogen_launchpad/source/sys_pmu.asm" 


