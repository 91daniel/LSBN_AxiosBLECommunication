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
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/home/tomwilliamson/Libraries/metrology/include -I"/home/tomwilliamson/workspace/Geometry/src" -I"/home/tomwilliamson/workspace/ipadNav_ble/include/tracking" -I"/home/tomwilliamson/workspace/ipadNav_ble/include/xml" -I"/home/tomwilliamson/workspace/ipadNav_ble/include/externalComms" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


