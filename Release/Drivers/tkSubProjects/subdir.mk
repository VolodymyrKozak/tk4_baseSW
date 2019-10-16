################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/tkSubProjects/pr_MBRTU_SLAVE.c \
../Drivers/tkSubProjects/pr_OVEN_MCD200.c \
../Drivers/tkSubProjects/pr_OVEN_cmi.c \
../Drivers/tkSubProjects/vv300Flash.c \
../Drivers/tkSubProjects/vv300set_factory.c \
../Drivers/tkSubProjects/vv300set_user.c \
../Drivers/tkSubProjects/wk_atime64.c 

OBJS += \
./Drivers/tkSubProjects/pr_MBRTU_SLAVE.o \
./Drivers/tkSubProjects/pr_OVEN_MCD200.o \
./Drivers/tkSubProjects/pr_OVEN_cmi.o \
./Drivers/tkSubProjects/vv300Flash.o \
./Drivers/tkSubProjects/vv300set_factory.o \
./Drivers/tkSubProjects/vv300set_user.o \
./Drivers/tkSubProjects/wk_atime64.o 

C_DEPS += \
./Drivers/tkSubProjects/pr_MBRTU_SLAVE.d \
./Drivers/tkSubProjects/pr_OVEN_MCD200.d \
./Drivers/tkSubProjects/pr_OVEN_cmi.d \
./Drivers/tkSubProjects/vv300Flash.d \
./Drivers/tkSubProjects/vv300set_factory.d \
./Drivers/tkSubProjects/vv300set_user.d \
./Drivers/tkSubProjects/wk_atime64.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/tkSubProjects/%.o: ../Drivers/tkSubProjects/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mfloat-abi=soft '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F105xC -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Inc" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/tkLibraries/tkMenu" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/tkLibraries" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/tkLibraries/time64" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/tkLibraries/tkGrid" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/tkLibraries/tkModbus" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/tkPROJECT" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/tkSubProjects" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/STM32F1xx_HAL_Driver/Inc" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/CMSIS/Device/ST/STM32F1xx/Include" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/CMSIS/Include"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


