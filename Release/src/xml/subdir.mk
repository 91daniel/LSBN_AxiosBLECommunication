################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/xml/pugixml.cpp 

OBJS += \
./src/xml/pugixml.o 

CPP_DEPS += \
./src/xml/pugixml.d 


# Each subdirectory must supply rules for building sources it contributes
src/xml/%.o: ../src/xml/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/home/tomwilliamson/Libraries/metrology/include -I"/home/tomwilliamson/workspace/Geometry/src" -I"/home/tomwilliamson/workspace/ipadNav_ble/include/tracking" -I"/home/tomwilliamson/workspace/ipadNav_ble/include/xml" -I"/home/tomwilliamson/workspace/ipadNav_ble/include/externalComms" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


