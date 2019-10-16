################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/tkLibraries/time64/stm32_rtc.c \
../Drivers/tkLibraries/time64/time64.c 

OBJS += \
./Drivers/tkLibraries/time64/stm32_rtc.o \
./Drivers/tkLibraries/time64/time64.o 

C_DEPS += \
./Drivers/tkLibraries/time64/stm32_rtc.d \
./Drivers/tkLibraries/time64/time64.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/tkLibraries/time64/%.o: ../Drivers/tkLibraries/time64/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mfloat-abi=soft '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F105xC -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Inc" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/tkLibraries/tkMenu" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/tkLibraries" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/tkLibraries/time64" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/tkLibraries/tkGrid" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/tkLibraries/tkModbus" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/tkPROJECT" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/tkSubProjects" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/STM32F1xx_HAL_Driver/Inc" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/CMSIS/Device/ST/STM32F1xx/Include" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/CMSIS/Include"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


