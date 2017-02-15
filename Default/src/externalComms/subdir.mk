################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/externalComms/cmd_def.c \
../src/externalComms/commands.c \
../src/externalComms/uart.c 

OBJS += \
./src/externalComms/cmd_def.o \
./src/externalComms/commands.o \
./src/externalComms/uart.o 

C_DEPS += \
./src/externalComms/cmd_def.d \
./src/externalComms/commands.d \
./src/externalComms/uart.d 


# Each subdirectory must supply rules for building sources it contributes
src/externalComms/%.o: ../src/externalComms/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -I"/home/daniel/workspace/Geometry" -I"/home/daniel/workspace/Metrology" -I"/home/daniel/workspace/ipadNav_ble/include" -I"/home/daniel/workspace/ipadNav_ble/include/tracking" -I"/home/daniel/workspace/ipadNav_ble/include/xml" -I"/home/daniel/workspace/ipadNav_ble/include/externalComms" -I"/home/daniel/workspace/PugiXML/src" -I"/home/daniel/workspace/ipadNav_ble/logging" -O2 -g -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


