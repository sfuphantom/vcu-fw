################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Add inputs and outputs from these tool invocations to the build variables 
CMD_SRCS += \
../halcogen_vcu/source/sys_linker_freeRTOS.cmd 

ASM_SRCS += \
../halcogen_vcu/source/dabort.asm \
../halcogen_vcu/source/os_portasm.asm \
../halcogen_vcu/source/sys_core.asm \
../halcogen_vcu/source/sys_intvecs.asm \
../halcogen_vcu/source/sys_mpu.asm \
../halcogen_vcu/source/sys_pmu.asm 

C_SRCS += \
../halcogen_vcu/source/Device_TMS570LS12.c \
../halcogen_vcu/source/Fapi_UserDefinedFunctions.c \
../halcogen_vcu/source/adc.c \
../halcogen_vcu/source/errata_SSWF021_45.c \
../halcogen_vcu/source/esm.c \
../halcogen_vcu/source/gio.c \
../halcogen_vcu/source/het.c \
../halcogen_vcu/source/i2c.c \
../halcogen_vcu/source/mibspi.c \
../halcogen_vcu/source/notification.c \
../halcogen_vcu/source/os_croutine.c \
../halcogen_vcu/source/os_event_groups.c \
../halcogen_vcu/source/os_heap.c \
../halcogen_vcu/source/os_list.c \
../halcogen_vcu/source/os_mpu_wrappers.c \
../halcogen_vcu/source/os_port.c \
../halcogen_vcu/source/os_queue.c \
../halcogen_vcu/source/os_tasks.c \
../halcogen_vcu/source/os_timer.c \
../halcogen_vcu/source/pinmux.c \
../halcogen_vcu/source/rti.c \
../halcogen_vcu/source/sci.c \
../halcogen_vcu/source/sys_dma.c \
../halcogen_vcu/source/sys_pcr.c \
../halcogen_vcu/source/sys_phantom.c \
../halcogen_vcu/source/sys_pmm.c \
../halcogen_vcu/source/sys_selftest.c \
../halcogen_vcu/source/sys_startup.c \
../halcogen_vcu/source/sys_vim.c \
../halcogen_vcu/source/system.c \
../halcogen_vcu/source/ti_fee_Info.c \
../halcogen_vcu/source/ti_fee_cancel.c \
../halcogen_vcu/source/ti_fee_cfg.c \
../halcogen_vcu/source/ti_fee_eraseimmediateblock.c \
../halcogen_vcu/source/ti_fee_format.c \
../halcogen_vcu/source/ti_fee_ini.c \
../halcogen_vcu/source/ti_fee_invalidateblock.c \
../halcogen_vcu/source/ti_fee_main.c \
../halcogen_vcu/source/ti_fee_read.c \
../halcogen_vcu/source/ti_fee_readSync.c \
../halcogen_vcu/source/ti_fee_shutdown.c \
../halcogen_vcu/source/ti_fee_util.c \
../halcogen_vcu/source/ti_fee_writeAsync.c \
../halcogen_vcu/source/ti_fee_writeSync.c 

C_DEPS += \
./halcogen_vcu/source/Device_TMS570LS12.d \
./halcogen_vcu/source/Fapi_UserDefinedFunctions.d \
./halcogen_vcu/source/adc.d \
./halcogen_vcu/source/errata_SSWF021_45.d \
./halcogen_vcu/source/esm.d \
./halcogen_vcu/source/gio.d \
./halcogen_vcu/source/het.d \
./halcogen_vcu/source/i2c.d \
./halcogen_vcu/source/mibspi.d \
./halcogen_vcu/source/notification.d \
./halcogen_vcu/source/os_croutine.d \
./halcogen_vcu/source/os_event_groups.d \
./halcogen_vcu/source/os_heap.d \
./halcogen_vcu/source/os_list.d \
./halcogen_vcu/source/os_mpu_wrappers.d \
./halcogen_vcu/source/os_port.d \
./halcogen_vcu/source/os_queue.d \
./halcogen_vcu/source/os_tasks.d \
./halcogen_vcu/source/os_timer.d \
./halcogen_vcu/source/pinmux.d \
./halcogen_vcu/source/rti.d \
./halcogen_vcu/source/sci.d \
./halcogen_vcu/source/sys_dma.d \
./halcogen_vcu/source/sys_pcr.d \
./halcogen_vcu/source/sys_phantom.d \
./halcogen_vcu/source/sys_pmm.d \
./halcogen_vcu/source/sys_selftest.d \
./halcogen_vcu/source/sys_startup.d \
./halcogen_vcu/source/sys_vim.d \
./halcogen_vcu/source/system.d \
./halcogen_vcu/source/ti_fee_Info.d \
./halcogen_vcu/source/ti_fee_cancel.d \
./halcogen_vcu/source/ti_fee_cfg.d \
./halcogen_vcu/source/ti_fee_eraseimmediateblock.d \
./halcogen_vcu/source/ti_fee_format.d \
./halcogen_vcu/source/ti_fee_ini.d \
./halcogen_vcu/source/ti_fee_invalidateblock.d \
./halcogen_vcu/source/ti_fee_main.d \
./halcogen_vcu/source/ti_fee_read.d \
./halcogen_vcu/source/ti_fee_readSync.d \
./halcogen_vcu/source/ti_fee_shutdown.d \
./halcogen_vcu/source/ti_fee_util.d \
./halcogen_vcu/source/ti_fee_writeAsync.d \
./halcogen_vcu/source/ti_fee_writeSync.d 

OBJS += \
./halcogen_vcu/source/Device_TMS570LS12.obj \
./halcogen_vcu/source/Fapi_UserDefinedFunctions.obj \
./halcogen_vcu/source/adc.obj \
./halcogen_vcu/source/dabort.obj \
./halcogen_vcu/source/errata_SSWF021_45.obj \
./halcogen_vcu/source/esm.obj \
./halcogen_vcu/source/gio.obj \
./halcogen_vcu/source/het.obj \
./halcogen_vcu/source/i2c.obj \
./halcogen_vcu/source/mibspi.obj \
./halcogen_vcu/source/notification.obj \
./halcogen_vcu/source/os_croutine.obj \
./halcogen_vcu/source/os_event_groups.obj \
./halcogen_vcu/source/os_heap.obj \
./halcogen_vcu/source/os_list.obj \
./halcogen_vcu/source/os_mpu_wrappers.obj \
./halcogen_vcu/source/os_port.obj \
./halcogen_vcu/source/os_portasm.obj \
./halcogen_vcu/source/os_queue.obj \
./halcogen_vcu/source/os_tasks.obj \
./halcogen_vcu/source/os_timer.obj \
./halcogen_vcu/source/pinmux.obj \
./halcogen_vcu/source/rti.obj \
./halcogen_vcu/source/sci.obj \
./halcogen_vcu/source/sys_core.obj \
./halcogen_vcu/source/sys_dma.obj \
./halcogen_vcu/source/sys_intvecs.obj \
./halcogen_vcu/source/sys_mpu.obj \
./halcogen_vcu/source/sys_pcr.obj \
./halcogen_vcu/source/sys_phantom.obj \
./halcogen_vcu/source/sys_pmm.obj \
./halcogen_vcu/source/sys_pmu.obj \
./halcogen_vcu/source/sys_selftest.obj \
./halcogen_vcu/source/sys_startup.obj \
./halcogen_vcu/source/sys_vim.obj \
./halcogen_vcu/source/system.obj \
./halcogen_vcu/source/ti_fee_Info.obj \
./halcogen_vcu/source/ti_fee_cancel.obj \
./halcogen_vcu/source/ti_fee_cfg.obj \
./halcogen_vcu/source/ti_fee_eraseimmediateblock.obj \
./halcogen_vcu/source/ti_fee_format.obj \
./halcogen_vcu/source/ti_fee_ini.obj \
./halcogen_vcu/source/ti_fee_invalidateblock.obj \
./halcogen_vcu/source/ti_fee_main.obj \
./halcogen_vcu/source/ti_fee_read.obj \
./halcogen_vcu/source/ti_fee_readSync.obj \
./halcogen_vcu/source/ti_fee_shutdown.obj \
./halcogen_vcu/source/ti_fee_util.obj \
./halcogen_vcu/source/ti_fee_writeAsync.obj \
./halcogen_vcu/source/ti_fee_writeSync.obj 

ASM_DEPS += \
./halcogen_vcu/source/dabort.d \
./halcogen_vcu/source/os_portasm.d \
./halcogen_vcu/source/sys_core.d \
./halcogen_vcu/source/sys_intvecs.d \
./halcogen_vcu/source/sys_mpu.d \
./halcogen_vcu/source/sys_pmu.d 

OBJS__QUOTED += \
"halcogen_vcu\source\Device_TMS570LS12.obj" \
"halcogen_vcu\source\Fapi_UserDefinedFunctions.obj" \
"halcogen_vcu\source\adc.obj" \
"halcogen_vcu\source\dabort.obj" \
"halcogen_vcu\source\errata_SSWF021_45.obj" \
"halcogen_vcu\source\esm.obj" \
"halcogen_vcu\source\gio.obj" \
"halcogen_vcu\source\het.obj" \
"halcogen_vcu\source\i2c.obj" \
"halcogen_vcu\source\mibspi.obj" \
"halcogen_vcu\source\notification.obj" \
"halcogen_vcu\source\os_croutine.obj" \
"halcogen_vcu\source\os_event_groups.obj" \
"halcogen_vcu\source\os_heap.obj" \
"halcogen_vcu\source\os_list.obj" \
"halcogen_vcu\source\os_mpu_wrappers.obj" \
"halcogen_vcu\source\os_port.obj" \
"halcogen_vcu\source\os_portasm.obj" \
"halcogen_vcu\source\os_queue.obj" \
"halcogen_vcu\source\os_tasks.obj" \
"halcogen_vcu\source\os_timer.obj" \
"halcogen_vcu\source\pinmux.obj" \
"halcogen_vcu\source\rti.obj" \
"halcogen_vcu\source\sci.obj" \
"halcogen_vcu\source\sys_core.obj" \
"halcogen_vcu\source\sys_dma.obj" \
"halcogen_vcu\source\sys_intvecs.obj" \
"halcogen_vcu\source\sys_mpu.obj" \
"halcogen_vcu\source\sys_pcr.obj" \
"halcogen_vcu\source\sys_phantom.obj" \
"halcogen_vcu\source\sys_pmm.obj" \
"halcogen_vcu\source\sys_pmu.obj" \
"halcogen_vcu\source\sys_selftest.obj" \
"halcogen_vcu\source\sys_startup.obj" \
"halcogen_vcu\source\sys_vim.obj" \
"halcogen_vcu\source\system.obj" \
"halcogen_vcu\source\ti_fee_Info.obj" \
"halcogen_vcu\source\ti_fee_cancel.obj" \
"halcogen_vcu\source\ti_fee_cfg.obj" \
"halcogen_vcu\source\ti_fee_eraseimmediateblock.obj" \
"halcogen_vcu\source\ti_fee_format.obj" \
"halcogen_vcu\source\ti_fee_ini.obj" \
"halcogen_vcu\source\ti_fee_invalidateblock.obj" \
"halcogen_vcu\source\ti_fee_main.obj" \
"halcogen_vcu\source\ti_fee_read.obj" \
"halcogen_vcu\source\ti_fee_readSync.obj" \
"halcogen_vcu\source\ti_fee_shutdown.obj" \
"halcogen_vcu\source\ti_fee_util.obj" \
"halcogen_vcu\source\ti_fee_writeAsync.obj" \
"halcogen_vcu\source\ti_fee_writeSync.obj" 

C_DEPS__QUOTED += \
"halcogen_vcu\source\Device_TMS570LS12.d" \
"halcogen_vcu\source\Fapi_UserDefinedFunctions.d" \
"halcogen_vcu\source\adc.d" \
"halcogen_vcu\source\errata_SSWF021_45.d" \
"halcogen_vcu\source\esm.d" \
"halcogen_vcu\source\gio.d" \
"halcogen_vcu\source\het.d" \
"halcogen_vcu\source\i2c.d" \
"halcogen_vcu\source\mibspi.d" \
"halcogen_vcu\source\notification.d" \
"halcogen_vcu\source\os_croutine.d" \
"halcogen_vcu\source\os_event_groups.d" \
"halcogen_vcu\source\os_heap.d" \
"halcogen_vcu\source\os_list.d" \
"halcogen_vcu\source\os_mpu_wrappers.d" \
"halcogen_vcu\source\os_port.d" \
"halcogen_vcu\source\os_queue.d" \
"halcogen_vcu\source\os_tasks.d" \
"halcogen_vcu\source\os_timer.d" \
"halcogen_vcu\source\pinmux.d" \
"halcogen_vcu\source\rti.d" \
"halcogen_vcu\source\sci.d" \
"halcogen_vcu\source\sys_dma.d" \
"halcogen_vcu\source\sys_pcr.d" \
"halcogen_vcu\source\sys_phantom.d" \
"halcogen_vcu\source\sys_pmm.d" \
"halcogen_vcu\source\sys_selftest.d" \
"halcogen_vcu\source\sys_startup.d" \
"halcogen_vcu\source\sys_vim.d" \
"halcogen_vcu\source\system.d" \
"halcogen_vcu\source\ti_fee_Info.d" \
"halcogen_vcu\source\ti_fee_cancel.d" \
"halcogen_vcu\source\ti_fee_cfg.d" \
"halcogen_vcu\source\ti_fee_eraseimmediateblock.d" \
"halcogen_vcu\source\ti_fee_format.d" \
"halcogen_vcu\source\ti_fee_ini.d" \
"halcogen_vcu\source\ti_fee_invalidateblock.d" \
"halcogen_vcu\source\ti_fee_main.d" \
"halcogen_vcu\source\ti_fee_read.d" \
"halcogen_vcu\source\ti_fee_readSync.d" \
"halcogen_vcu\source\ti_fee_shutdown.d" \
"halcogen_vcu\source\ti_fee_util.d" \
"halcogen_vcu\source\ti_fee_writeAsync.d" \
"halcogen_vcu\source\ti_fee_writeSync.d" 

ASM_DEPS__QUOTED += \
"halcogen_vcu\source\dabort.d" \
"halcogen_vcu\source\os_portasm.d" \
"halcogen_vcu\source\sys_core.d" \
"halcogen_vcu\source\sys_intvecs.d" \
"halcogen_vcu\source\sys_mpu.d" \
"halcogen_vcu\source\sys_pmu.d" 

C_SRCS__QUOTED += \
"../halcogen_vcu/source/Device_TMS570LS12.c" \
"../halcogen_vcu/source/Fapi_UserDefinedFunctions.c" \
"../halcogen_vcu/source/adc.c" \
"../halcogen_vcu/source/errata_SSWF021_45.c" \
"../halcogen_vcu/source/esm.c" \
"../halcogen_vcu/source/gio.c" \
"../halcogen_vcu/source/het.c" \
"../halcogen_vcu/source/i2c.c" \
"../halcogen_vcu/source/mibspi.c" \
"../halcogen_vcu/source/notification.c" \
"../halcogen_vcu/source/os_croutine.c" \
"../halcogen_vcu/source/os_event_groups.c" \
"../halcogen_vcu/source/os_heap.c" \
"../halcogen_vcu/source/os_list.c" \
"../halcogen_vcu/source/os_mpu_wrappers.c" \
"../halcogen_vcu/source/os_port.c" \
"../halcogen_vcu/source/os_queue.c" \
"../halcogen_vcu/source/os_tasks.c" \
"../halcogen_vcu/source/os_timer.c" \
"../halcogen_vcu/source/pinmux.c" \
"../halcogen_vcu/source/rti.c" \
"../halcogen_vcu/source/sci.c" \
"../halcogen_vcu/source/sys_dma.c" \
"../halcogen_vcu/source/sys_pcr.c" \
"../halcogen_vcu/source/sys_phantom.c" \
"../halcogen_vcu/source/sys_pmm.c" \
"../halcogen_vcu/source/sys_selftest.c" \
"../halcogen_vcu/source/sys_startup.c" \
"../halcogen_vcu/source/sys_vim.c" \
"../halcogen_vcu/source/system.c" \
"../halcogen_vcu/source/ti_fee_Info.c" \
"../halcogen_vcu/source/ti_fee_cancel.c" \
"../halcogen_vcu/source/ti_fee_cfg.c" \
"../halcogen_vcu/source/ti_fee_eraseimmediateblock.c" \
"../halcogen_vcu/source/ti_fee_format.c" \
"../halcogen_vcu/source/ti_fee_ini.c" \
"../halcogen_vcu/source/ti_fee_invalidateblock.c" \
"../halcogen_vcu/source/ti_fee_main.c" \
"../halcogen_vcu/source/ti_fee_read.c" \
"../halcogen_vcu/source/ti_fee_readSync.c" \
"../halcogen_vcu/source/ti_fee_shutdown.c" \
"../halcogen_vcu/source/ti_fee_util.c" \
"../halcogen_vcu/source/ti_fee_writeAsync.c" \
"../halcogen_vcu/source/ti_fee_writeSync.c" 

ASM_SRCS__QUOTED += \
"../halcogen_vcu/source/dabort.asm" \
"../halcogen_vcu/source/os_portasm.asm" \
"../halcogen_vcu/source/sys_core.asm" \
"../halcogen_vcu/source/sys_intvecs.asm" \
"../halcogen_vcu/source/sys_mpu.asm" \
"../halcogen_vcu/source/sys_pmu.asm" 


