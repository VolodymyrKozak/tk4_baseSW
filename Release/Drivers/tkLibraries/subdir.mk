################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/tkLibraries/tk0flash_base.c \
../Drivers/tkLibraries/tk1flash2page_read.c \
../Drivers/tkLibraries/tk1flash2page_write.c \
../Drivers/tkLibraries/tk2_ModbusControl.c \
../Drivers/tkLibraries/tkDebugThings.c \
../Drivers/tkLibraries/tkErrLib.c \
../Drivers/tkLibraries/tkSGL.c \
../Drivers/tkLibraries/tk_oven_smi2.c \
../Drivers/tkLibraries/tk_wh1602b3.c \
../Drivers/tkLibraries/tk_wo1602g.c \
../Drivers/tkLibraries/vkButton5ms.c \
../Drivers/tkLibraries/vkButtonSmooth.c 

OBJS += \
./Drivers/tkLibraries/tk0flash_base.o \
./Drivers/tkLibraries/tk1flash2page_read.o \
./Drivers/tkLibraries/tk1flash2page_write.o \
./Drivers/tkLibraries/tk2_ModbusControl.o \
./Drivers/tkLibraries/tkDebugThings.o \
./Drivers/tkLibraries/tkErrLib.o \
./Drivers/tkLibraries/tkSGL.o \
./Drivers/tkLibraries/tk_oven_smi2.o \
./Drivers/tkLibraries/tk_wh1602b3.o \
./Drivers/tkLibraries/tk_wo1602g.o \
./Drivers/tkLibraries/vkButton5ms.o \
./Drivers/tkLibraries/vkButtonSmooth.o 

C_DEPS += \
./Drivers/tkLibraries/tk0flash_base.d \
./Drivers/tkLibraries/tk1flash2page_read.d \
./Drivers/tkLibraries/tk1flash2page_write.d \
./Drivers/tkLibraries/tk2_ModbusControl.d \
./Drivers/tkLibraries/tkDebugThings.d \
./Drivers/tkLibraries/tkErrLib.d \
./Drivers/tkLibraries/tkSGL.d \
./Drivers/tkLibraries/tk_oven_smi2.d \
./Drivers/tkLibraries/tk_wh1602b3.d \
./Drivers/tkLibraries/tk_wo1602g.d \
./Drivers/tkLibraries/vkButton5ms.d \
./Drivers/tkLibraries/vkButtonSmooth.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/tkLibraries/%.o: ../Drivers/tkLibraries/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mfloat-abi=soft '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F105xC -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Inc" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/tkLibraries/tkMenu" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/tkLibraries" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/tkLibraries/time64" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/tkLibraries/tkGrid" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/tkLibraries/tkModbus" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/tkPROJECT" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/tkSubProjects" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/STM32F1xx_HAL_Driver/Inc" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/CMSIS/Device/ST/STM32F1xx/Include" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/CMSIS/Include"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


