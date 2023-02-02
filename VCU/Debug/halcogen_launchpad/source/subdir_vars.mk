################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Add inputs and outputs from these tool invocations to the build variables 
CMD_SRCS += \
../halcogen_launchpad/source/sys_link.cmd 

ASM_SRCS += \
../halcogen_launchpad/source/dabort.asm \
../halcogen_launchpad/source/os_portasm.asm \
../halcogen_launchpad/source/svc.asm \
../halcogen_launchpad/source/sys_core.asm \
../halcogen_launchpad/source/sys_intvecs.asm \
../halcogen_launchpad/source/sys_mpu.asm \
../halcogen_launchpad/source/sys_pmu.asm 

C_SRCS += \
../halcogen_launchpad/source/Device_TMS570LS12.c \
../halcogen_launchpad/source/Fapi_UserDefinedFunctions.c \
../halcogen_launchpad/source/adc.c \
../halcogen_launchpad/source/errata_SSWF021_45.c \
../halcogen_launchpad/source/esm.c \
../halcogen_launchpad/source/gio.c \
../halcogen_launchpad/source/het.c \
../halcogen_launchpad/source/i2c.c \
../halcogen_launchpad/source/mibspi.c \
../halcogen_launchpad/source/notification.c \
../halcogen_launchpad/source/os_croutine.c \
../halcogen_launchpad/source/os_event_groups.c \
../halcogen_launchpad/source/os_heap.c \
../halcogen_launchpad/source/os_list.c \
../halcogen_launchpad/source/os_mpu_wrappers.c \
../halcogen_launchpad/source/os_port.c \
../halcogen_launchpad/source/os_queue.c \
../halcogen_launchpad/source/os_tasks.c \
../halcogen_launchpad/source/os_timer.c \
../halcogen_launchpad/source/pinmux.c \
../halcogen_launchpad/source/rti.c \
../halcogen_launchpad/source/sci.c \
../halcogen_launchpad/source/sys_dma.c \
../halcogen_launchpad/source/sys_main.c \
../halcogen_launchpad/source/sys_pcr.c \
../halcogen_launchpad/source/sys_phantom.c \
../halcogen_launchpad/source/sys_pmm.c \
../halcogen_launchpad/source/sys_selftest.c \
../halcogen_launchpad/source/sys_startup.c \
../halcogen_launchpad/source/sys_vim.c \
../halcogen_launchpad/source/system.c \
../halcogen_launchpad/source/ti_fee_Info.c \
../halcogen_launchpad/source/ti_fee_cancel.c \
../halcogen_launchpad/source/ti_fee_cfg.c \
../halcogen_launchpad/source/ti_fee_eraseimmediateblock.c \
../halcogen_launchpad/source/ti_fee_format.c \
../halcogen_launchpad/source/ti_fee_ini.c \
../halcogen_launchpad/source/ti_fee_invalidateblock.c \
../halcogen_launchpad/source/ti_fee_main.c \
../halcogen_launchpad/source/ti_fee_read.c \
../halcogen_launchpad/source/ti_fee_readSync.c \
../halcogen_launchpad/source/ti_fee_shutdown.c \
../halcogen_launchpad/source/ti_fee_util.c \
../halcogen_launchpad/source/ti_fee_writeAsync.c \
../halcogen_launchpad/source/ti_fee_writeSync.c 

C_DEPS += \
./halcogen_launchpad/source/Device_TMS570LS12.d \
./halcogen_launchpad/source/Fapi_UserDefinedFunctions.d \
./halcogen_launchpad/source/adc.d \
./halcogen_launchpad/source/errata_SSWF021_45.d \
./halcogen_launchpad/source/esm.d \
./halcogen_launchpad/source/gio.d \
./halcogen_launchpad/source/het.d \
./halcogen_launchpad/source/i2c.d \
./halcogen_launchpad/source/mibspi.d \
./halcogen_launchpad/source/notification.d \
./halcogen_launchpad/source/os_croutine.d \
./halcogen_launchpad/source/os_event_groups.d \
./halcogen_launchpad/source/os_heap.d \
./halcogen_launchpad/source/os_list.d \
./halcogen_launchpad/source/os_mpu_wrappers.d \
./halcogen_launchpad/source/os_port.d \
./halcogen_launchpad/source/os_queue.d \
./halcogen_launchpad/source/os_tasks.d \
./halcogen_launchpad/source/os_timer.d \
./halcogen_launchpad/source/pinmux.d \
./halcogen_launchpad/source/rti.d \
./halcogen_launchpad/source/sci.d \
./halcogen_launchpad/source/sys_dma.d \
./halcogen_launchpad/source/sys_main.d \
./halcogen_launchpad/source/sys_pcr.d \
./halcogen_launchpad/source/sys_phantom.d \
./halcogen_launchpad/source/sys_pmm.d \
./halcogen_launchpad/source/sys_selftest.d \
./halcogen_launchpad/source/sys_startup.d \
./halcogen_launchpad/source/sys_vim.d \
./halcogen_launchpad/source/system.d \
./halcogen_launchpad/source/ti_fee_Info.d \
./halcogen_launchpad/source/ti_fee_cancel.d \
./halcogen_launchpad/source/ti_fee_cfg.d \
./halcogen_launchpad/source/ti_fee_eraseimmediateblock.d \
./halcogen_launchpad/source/ti_fee_format.d \
./halcogen_launchpad/source/ti_fee_ini.d \
./halcogen_launchpad/source/ti_fee_invalidateblock.d \
./halcogen_launchpad/source/ti_fee_main.d \
./halcogen_launchpad/source/ti_fee_read.d \
./halcogen_launchpad/source/ti_fee_readSync.d \
./halcogen_launchpad/source/ti_fee_shutdown.d \
./halcogen_launchpad/source/ti_fee_util.d \
./halcogen_launchpad/source/ti_fee_writeAsync.d \
./halcogen_launchpad/source/ti_fee_writeSync.d 

OBJS += \
./halcogen_launchpad/source/Device_TMS570LS12.obj \
./halcogen_launchpad/source/Fapi_UserDefinedFunctions.obj \
./halcogen_launchpad/source/adc.obj \
./halcogen_launchpad/source/dabort.obj \
./halcogen_launchpad/source/errata_SSWF021_45.obj \
./halcogen_launchpad/source/esm.obj \
./halcogen_launchpad/source/gio.obj \
./halcogen_launchpad/source/het.obj \
./halcogen_launchpad/source/i2c.obj \
./halcogen_launchpad/source/mibspi.obj \
./halcogen_launchpad/source/notification.obj \
./halcogen_launchpad/source/os_croutine.obj \
./halcogen_launchpad/source/os_event_groups.obj \
./halcogen_launchpad/source/os_heap.obj \
./halcogen_launchpad/source/os_list.obj \
./halcogen_launchpad/source/os_mpu_wrappers.obj \
./halcogen_launchpad/source/os_port.obj \
./halcogen_launchpad/source/os_portasm.obj \
./halcogen_launchpad/source/os_queue.obj \
./halcogen_launchpad/source/os_tasks.obj \
./halcogen_launchpad/source/os_timer.obj \
./halcogen_launchpad/source/pinmux.obj \
./halcogen_launchpad/source/rti.obj \
./halcogen_launchpad/source/sci.obj \
./halcogen_launchpad/source/svc.obj \
./halcogen_launchpad/source/sys_core.obj \
./halcogen_launchpad/source/sys_dma.obj \
./halcogen_launchpad/source/sys_intvecs.obj \
./halcogen_launchpad/source/sys_main.obj \
./halcogen_launchpad/source/sys_mpu.obj \
./halcogen_launchpad/source/sys_pcr.obj \
./halcogen_launchpad/source/sys_phantom.obj \
./halcogen_launchpad/source/sys_pmm.obj \
./halcogen_launchpad/source/sys_pmu.obj \
./halcogen_launchpad/source/sys_selftest.obj \
./halcogen_launchpad/source/sys_startup.obj \
./halcogen_launchpad/source/sys_vim.obj \
./halcogen_launchpad/source/system.obj \
./halcogen_launchpad/source/ti_fee_Info.obj \
./halcogen_launchpad/source/ti_fee_cancel.obj \
./halcogen_launchpad/source/ti_fee_cfg.obj \
./halcogen_launchpad/source/ti_fee_eraseimmediateblock.obj \
./halcogen_launchpad/source/ti_fee_format.obj \
./halcogen_launchpad/source/ti_fee_ini.obj \
./halcogen_launchpad/source/ti_fee_invalidateblock.obj \
./halcogen_launchpad/source/ti_fee_main.obj \
./halcogen_launchpad/source/ti_fee_read.obj \
./halcogen_launchpad/source/ti_fee_readSync.obj \
./halcogen_launchpad/source/ti_fee_shutdown.obj \
./halcogen_launchpad/source/ti_fee_util.obj \
./halcogen_launchpad/source/ti_fee_writeAsync.obj \
./halcogen_launchpad/source/ti_fee_writeSync.obj 

ASM_DEPS += \
./halcogen_launchpad/source/dabort.d \
./halcogen_launchpad/source/os_portasm.d \
./halcogen_launchpad/source/svc.d \
./halcogen_launchpad/source/sys_core.d \
./halcogen_launchpad/source/sys_intvecs.d \
./halcogen_launchpad/source/sys_mpu.d \
./halcogen_launchpad/source/sys_pmu.d 

OBJS__QUOTED += \
"halcogen_launchpad\source\Device_TMS570LS12.obj" \
"halcogen_launchpad\source\Fapi_UserDefinedFunctions.obj" \
"halcogen_launchpad\source\adc.obj" \
"halcogen_launchpad\source\dabort.obj" \
"halcogen_launchpad\source\errata_SSWF021_45.obj" \
"halcogen_launchpad\source\esm.obj" \
"halcogen_launchpad\source\gio.obj" \
"halcogen_launchpad\source\het.obj" \
"halcogen_launchpad\source\i2c.obj" \
"halcogen_launchpad\source\mibspi.obj" \
"halcogen_launchpad\source\notification.obj" \
"halcogen_launchpad\source\os_croutine.obj" \
"halcogen_launchpad\source\os_event_groups.obj" \
"halcogen_launchpad\source\os_heap.obj" \
"halcogen_launchpad\source\os_list.obj" \
"halcogen_launchpad\source\os_mpu_wrappers.obj" \
"halcogen_launchpad\source\os_port.obj" \
"halcogen_launchpad\source\os_portasm.obj" \
"halcogen_launchpad\source\os_queue.obj" \
"halcogen_launchpad\source\os_tasks.obj" \
"halcogen_launchpad\source\os_timer.obj" \
"halcogen_launchpad\source\pinmux.obj" \
"halcogen_launchpad\source\rti.obj" \
"halcogen_launchpad\source\sci.obj" \
"halcogen_launchpad\source\svc.obj" \
"halcogen_launchpad\source\sys_core.obj" \
"halcogen_launchpad\source\sys_dma.obj" \
"halcogen_launchpad\source\sys_intvecs.obj" \
"halcogen_launchpad\source\sys_main.obj" \
"halcogen_launchpad\source\sys_mpu.obj" \
"halcogen_launchpad\source\sys_pcr.obj" \
"halcogen_launchpad\source\sys_phantom.obj" \
"halcogen_launchpad\source\sys_pmm.obj" \
"halcogen_launchpad\source\sys_pmu.obj" \
"halcogen_launchpad\source\sys_selftest.obj" \
"halcogen_launchpad\source\sys_startup.obj" \
"halcogen_launchpad\source\sys_vim.obj" \
"halcogen_launchpad\source\system.obj" \
"halcogen_launchpad\source\ti_fee_Info.obj" \
"halcogen_launchpad\source\ti_fee_cancel.obj" \
"halcogen_launchpad\source\ti_fee_cfg.obj" \
"halcogen_launchpad\source\ti_fee_eraseimmediateblock.obj" \
"halcogen_launchpad\source\ti_fee_format.obj" \
"halcogen_launchpad\source\ti_fee_ini.obj" \
"halcogen_launchpad\source\ti_fee_invalidateblock.obj" \
"halcogen_launchpad\source\ti_fee_main.obj" \
"halcogen_launchpad\source\ti_fee_read.obj" \
"halcogen_launchpad\source\ti_fee_readSync.obj" \
"halcogen_launchpad\source\ti_fee_shutdown.obj" \
"halcogen_launchpad\source\ti_fee_util.obj" \
"halcogen_launchpad\source\ti_fee_writeAsync.obj" \
"halcogen_launchpad\source\ti_fee_writeSync.obj" 

C_DEPS__QUOTED += \
"halcogen_launchpad\source\Device_TMS570LS12.d" \
"halcogen_launchpad\source\Fapi_UserDefinedFunctions.d" \
"halcogen_launchpad\source\adc.d" \
"halcogen_launchpad\source\errata_SSWF021_45.d" \
"halcogen_launchpad\source\esm.d" \
"halcogen_launchpad\source\gio.d" \
"halcogen_launchpad\source\het.d" \
"halcogen_launchpad\source\i2c.d" \
"halcogen_launchpad\source\mibspi.d" \
"halcogen_launchpad\source\notification.d" \
"halcogen_launchpad\source\os_croutine.d" \
"halcogen_launchpad\source\os_event_groups.d" \
"halcogen_launchpad\source\os_heap.d" \
"halcogen_launchpad\source\os_list.d" \
"halcogen_launchpad\source\os_mpu_wrappers.d" \
"halcogen_launchpad\source\os_port.d" \
"halcogen_launchpad\source\os_queue.d" \
"halcogen_launchpad\source\os_tasks.d" \
"halcogen_launchpad\source\os_timer.d" \
"halcogen_launchpad\source\pinmux.d" \
"halcogen_launchpad\source\rti.d" \
"halcogen_launchpad\source\sci.d" \
"halcogen_launchpad\source\sys_dma.d" \
"halcogen_launchpad\source\sys_main.d" \
"halcogen_launchpad\source\sys_pcr.d" \
"halcogen_launchpad\source\sys_phantom.d" \
"halcogen_launchpad\source\sys_pmm.d" \
"halcogen_launchpad\source\sys_selftest.d" \
"halcogen_launchpad\source\sys_startup.d" \
"halcogen_launchpad\source\sys_vim.d" \
"halcogen_launchpad\source\system.d" \
"halcogen_launchpad\source\ti_fee_Info.d" \
"halcogen_launchpad\source\ti_fee_cancel.d" \
"halcogen_launchpad\source\ti_fee_cfg.d" \
"halcogen_launchpad\source\ti_fee_eraseimmediateblock.d" \
"halcogen_launchpad\source\ti_fee_format.d" \
"halcogen_launchpad\source\ti_fee_ini.d" \
"halcogen_launchpad\source\ti_fee_invalidateblock.d" \
"halcogen_launchpad\source\ti_fee_main.d" \
"halcogen_launchpad\source\ti_fee_read.d" \
"halcogen_launchpad\source\ti_fee_readSync.d" \
"halcogen_launchpad\source\ti_fee_shutdown.d" \
"halcogen_launchpad\source\ti_fee_util.d" \
"halcogen_launchpad\source\ti_fee_writeAsync.d" \
"halcogen_launchpad\source\ti_fee_writeSync.d" 

ASM_DEPS__QUOTED += \
"halcogen_launchpad\source\dabort.d" \
"halcogen_launchpad\source\os_portasm.d" \
"halcogen_launchpad\source\svc.d" \
"halcogen_launchpad\source\sys_core.d" \
"halcogen_launchpad\source\sys_intvecs.d" \
"halcogen_launchpad\source\sys_mpu.d" \
"halcogen_launchpad\source\sys_pmu.d" 

C_SRCS__QUOTED += \
"../halcogen_launchpad/source/Device_TMS570LS12.c" \
"../halcogen_launchpad/source/Fapi_UserDefinedFunctions.c" \
"../halcogen_launchpad/source/adc.c" \
"../halcogen_launchpad/source/errata_SSWF021_45.c" \
"../halcogen_launchpad/source/esm.c" \
"../halcogen_launchpad/source/gio.c" \
"../halcogen_launchpad/source/het.c" \
"../halcogen_launchpad/source/i2c.c" \
"../halcogen_launchpad/source/mibspi.c" \
"../halcogen_launchpad/source/notification.c" \
"../halcogen_launchpad/source/os_croutine.c" \
"../halcogen_launchpad/source/os_event_groups.c" \
"../halcogen_launchpad/source/os_heap.c" \
"../halcogen_launchpad/source/os_list.c" \
"../halcogen_launchpad/source/os_mpu_wrappers.c" \
"../halcogen_launchpad/source/os_port.c" \
"../halcogen_launchpad/source/os_queue.c" \
"../halcogen_launchpad/source/os_tasks.c" \
"../halcogen_launchpad/source/os_timer.c" \
"../halcogen_launchpad/source/pinmux.c" \
"../halcogen_launchpad/source/rti.c" \
"../halcogen_launchpad/source/sci.c" \
"../halcogen_launchpad/source/sys_dma.c" \
"../halcogen_launchpad/source/sys_main.c" \
"../halcogen_launchpad/source/sys_pcr.c" \
"../halcogen_launchpad/source/sys_phantom.c" \
"../halcogen_launchpad/source/sys_pmm.c" \
"../halcogen_launchpad/source/sys_selftest.c" \
"../halcogen_launchpad/source/sys_startup.c" \
"../halcogen_launchpad/source/sys_vim.c" \
"../halcogen_launchpad/source/system.c" \
"../halcogen_launchpad/source/ti_fee_Info.c" \
"../halcogen_launchpad/source/ti_fee_cancel.c" \
"../halcogen_launchpad/source/ti_fee_cfg.c" \
"../halcogen_launchpad/source/ti_fee_eraseimmediateblock.c" \
"../halcogen_launchpad/source/ti_fee_format.c" \
"../halcogen_launchpad/source/ti_fee_ini.c" \
"../halcogen_launchpad/source/ti_fee_invalidateblock.c" \
"../halcogen_launchpad/source/ti_fee_main.c" \
"../halcogen_launchpad/source/ti_fee_read.c" \
"../halcogen_launchpad/source/ti_fee_readSync.c" \
"../halcogen_launchpad/source/ti_fee_shutdown.c" \
"../halcogen_launchpad/source/ti_fee_util.c" \
"../halcogen_launchpad/source/ti_fee_writeAsync.c" \
"../halcogen_launchpad/source/ti_fee_writeSync.c" 

ASM_SRCS__QUOTED += \
"../halcogen_launchpad/source/dabort.asm" \
"../halcogen_launchpad/source/os_portasm.asm" \
"../halcogen_launchpad/source/svc.asm" \
"../halcogen_launchpad/source/sys_core.asm" \
"../halcogen_launchpad/source/sys_intvecs.asm" \
"../halcogen_launchpad/source/sys_mpu.asm" \
"../halcogen_launchpad/source/sys_pmu.asm" 


