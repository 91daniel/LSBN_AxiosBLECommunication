################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/tracking/marker.cpp \
../src/tracking/trackingDevice.cpp 

OBJS += \
./src/tracking/marker.o \
./src/tracking/trackingDevice.o 

CPP_DEPS += \
./src/tracking/marker.d \
./src/tracking/trackingDevice.d 


# Each subdirectory must supply rules for building sources it contributes
src/tracking/%.o: ../src/tracking/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I"/home/daniel/workspace/Geometry/src" -I"/home/daniel/workspace/Metrology/src" -I"/home/daniel/workspace/ipadNav_ble/include/externalComms" -I"/home/daniel/workspace/ipadNav_ble/include/tracking" -I"/home/daniel/workspace/ipadNav_ble/include/xml" -I"/home/daniel/workspace/PugiXML/src" -I"/home/daniel/workspace/ipadNav_ble/logging" -O2 -g -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

