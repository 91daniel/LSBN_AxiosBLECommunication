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
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I"/home/daniel/workspace/Geometry/src" -I"/home/daniel/workspace/Metrology/src" -I"/home/daniel/workspace/ipadNav_ble/include/externalComms" -I"/home/daniel/workspace/ipadNav_ble/include/tracking" -I"/home/daniel/workspace/ipadNav_ble/include/xml" -I"/home/daniel/workspace/PugiXML/src" -I"/home/daniel/workspace/ipadNav_ble/logging" -O2 -g -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


