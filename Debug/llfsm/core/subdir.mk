################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../llfsm/core/llContext.cpp \
../llfsm/core/llEvtData.cpp \
../llfsm/core/llFSM.cpp \
../llfsm/core/llRef.cpp \
../llfsm/core/llState.cpp \
../llfsm/core/llUtils.cpp 

OBJS += \
./llfsm/core/llContext.o \
./llfsm/core/llEvtData.o \
./llfsm/core/llFSM.o \
./llfsm/core/llRef.o \
./llfsm/core/llState.o \
./llfsm/core/llUtils.o 

CPP_DEPS += \
./llfsm/core/llContext.d \
./llfsm/core/llEvtData.d \
./llfsm/core/llFSM.d \
./llfsm/core/llRef.d \
./llfsm/core/llState.d \
./llfsm/core/llUtils.d 


# Each subdirectory must supply rules for building sources it contributes
llfsm/core/%.o: ../llfsm/core/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/home/CORPUSERS/28848747/eclipse-workspace/cobins/llfsm -I"/home/CORPUSERS/28848747/eclipse-workspace/cobins" -O0 -g3 -Wall -c -fmessage-length=0  -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


