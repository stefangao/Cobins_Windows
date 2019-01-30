################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../llfsm/platform/llTimer.cpp 

OBJS += \
./llfsm/platform/llTimer.o 

CPP_DEPS += \
./llfsm/platform/llTimer.d 


# Each subdirectory must supply rules for building sources it contributes
llfsm/platform/%.o: ../llfsm/platform/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/home/CORPUSERS/28848747/eclipse-workspace/cobins/llfsm -I"/home/CORPUSERS/28848747/eclipse-workspace/cobins" -O0 -g3 -Wall -c -fmessage-length=0  -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


