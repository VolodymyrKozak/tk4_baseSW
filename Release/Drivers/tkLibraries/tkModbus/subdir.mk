################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/tkLibraries/tkModbus/mb.c \
../Drivers/tkLibraries/tkModbus/mb_forTK2_master.c \
../Drivers/tkLibraries/tkModbus/mb_master.c \
../Drivers/tkLibraries/tkModbus/mb_master5.c \
../Drivers/tkLibraries/tkModbus/mb_slave.c \
../Drivers/tkLibraries/tkModbus/mb_slave5.c \
../Drivers/tkLibraries/tkModbus/mb_slave5a.c \
../Drivers/tkLibraries/tkModbus/tkCRC.c 

OBJS += \
./Drivers/tkLibraries/tkModbus/mb.o \
./Drivers/tkLibraries/tkModbus/mb_forTK2_master.o \
./Drivers/tkLibraries/tkModbus/mb_master.o \
./Drivers/tkLibraries/tkModbus/mb_master5.o \
./Drivers/tkLibraries/tkModbus/mb_slave.o \
./Drivers/tkLibraries/tkModbus/mb_slave5.o \
./Drivers/tkLibraries/tkModbus/mb_slave5a.o \
./Drivers/tkLibraries/tkModbus/tkCRC.o 

C_DEPS += \
./Drivers/tkLibraries/tkModbus/mb.d \
./Drivers/tkLibraries/tkModbus/mb_forTK2_master.d \
./Drivers/tkLibraries/tkModbus/mb_master.d \
./Drivers/tkLibraries/tkModbus/mb_master5.d \
./Drivers/tkLibraries/tkModbus/mb_slave.d \
./Drivers/tkLibraries/tkModbus/mb_slave5.d \
./Drivers/tkLibraries/tkModbus/mb_slave5a.d \
./Drivers/tkLibraries/tkModbus/tkCRC.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/tkLibraries/tkModbus/%.o: ../Drivers/tkLibraries/tkModbus/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mfloat-abi=soft '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F105xC -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Inc" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/tkLibraries/tkMenu" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/tkLibraries" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/tkLibraries/time64" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/tkLibraries/tkGrid" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/tkLibraries/tkModbus" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/tkPROJECT" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/tkSubProjects" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/STM32F1xx_HAL_Driver/Inc" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/CMSIS/Device/ST/STM32F1xx/Include" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/CMSIS/Include"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


