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
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/tomwilliamson/workspace/ipadNav_ble/include/externalComms" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


