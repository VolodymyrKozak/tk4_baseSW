################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/tkPROJECT/tkConfig.c \
../Drivers/tkPROJECT/tkErr_Handler.c \
../Drivers/tkPROJECT/tkMain.c \
../Drivers/tkPROJECT/vd_timemain.c \
../Drivers/tkPROJECT/zkComment.c 

OBJS += \
./Drivers/tkPROJECT/tkConfig.o \
./Drivers/tkPROJECT/tkErr_Handler.o \
./Drivers/tkPROJECT/tkMain.o \
./Drivers/tkPROJECT/vd_timemain.o \
./Drivers/tkPROJECT/zkComment.o 

C_DEPS += \
./Drivers/tkPROJECT/tkConfig.d \
./Drivers/tkPROJECT/tkErr_Handler.d \
./Drivers/tkPROJECT/tkMain.d \
./Drivers/tkPROJECT/vd_timemain.d \
./Drivers/tkPROJECT/zkComment.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/tkPROJECT/%.o: ../Drivers/tkPROJECT/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mfloat-abi=soft '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F105xC -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Inc" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/tkLibraries/tkMenu" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/tkLibraries" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/tkLibraries/time64" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/tkLibraries/tkGrid" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/tkLibraries/tkModbus" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/tkPROJECT" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/tkSubProjects" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/STM32F1xx_HAL_Driver/Inc" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/CMSIS/Device/ST/STM32F1xx/Include" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/CMSIS/Include"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


