################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/tkLibraries/tkMenu/tk0spv.c \
../Drivers/tkLibraries/tkMenu/tk1spv_grid.c \
../Drivers/tkLibraries/tkMenu/tk2spv_set.c \
../Drivers/tkLibraries/tkMenu/tkDTset.c \
../Drivers/tkLibraries/tkMenu/u16array_set.c \
../Drivers/tkLibraries/tkMenu/u16set.c \
../Drivers/tkLibraries/tkMenu/uList_set.c \
../Drivers/tkLibraries/tkMenu/zFF.c 

OBJS += \
./Drivers/tkLibraries/tkMenu/tk0spv.o \
./Drivers/tkLibraries/tkMenu/tk1spv_grid.o \
./Drivers/tkLibraries/tkMenu/tk2spv_set.o \
./Drivers/tkLibraries/tkMenu/tkDTset.o \
./Drivers/tkLibraries/tkMenu/u16array_set.o \
./Drivers/tkLibraries/tkMenu/u16set.o \
./Drivers/tkLibraries/tkMenu/uList_set.o \
./Drivers/tkLibraries/tkMenu/zFF.o 

C_DEPS += \
./Drivers/tkLibraries/tkMenu/tk0spv.d \
./Drivers/tkLibraries/tkMenu/tk1spv_grid.d \
./Drivers/tkLibraries/tkMenu/tk2spv_set.d \
./Drivers/tkLibraries/tkMenu/tkDTset.d \
./Drivers/tkLibraries/tkMenu/u16array_set.d \
./Drivers/tkLibraries/tkMenu/u16set.d \
./Drivers/tkLibraries/tkMenu/uList_set.d \
./Drivers/tkLibraries/tkMenu/zFF.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/tkLibraries/tkMenu/%.o: ../Drivers/tkLibraries/tkMenu/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mfloat-abi=soft '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F105xC -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Inc" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/tkLibraries/tkMenu" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/tkLibraries" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/tkLibraries/time64" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/tkLibraries/tkGrid" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/tkLibraries/tkModbus" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/tkPROJECT" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/tkSubProjects" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/STM32F1xx_HAL_Driver/Inc" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/CMSIS/Device/ST/STM32F1xx/Include" -I"C:/Users/KozakVF/workspace/190315_TK300_Office3/Drivers/CMSIS/Include"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


