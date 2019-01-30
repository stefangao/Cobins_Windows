################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../core/cobBin.cpp \
../core/cobContext.cpp \
../core/cobIProbe.cpp \
../core/cobProbe.cpp \
../core/cobProber.cpp \
../core/cobRobot.cpp 

OBJS += \
./core/cobBin.o \
./core/cobContext.o \
./core/cobIProbe.o \
./core/cobProbe.o \
./core/cobProber.o \
./core/cobRobot.o 

CPP_DEPS += \
./core/cobBin.d \
./core/cobContext.d \
./core/cobIProbe.d \
./core/cobProbe.d \
./core/cobProber.d \
./core/cobRobot.d 


# Each subdirectory must supply rules for building sources it contributes
core/%.o: ../core/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/home/CORPUSERS/28848747/eclipse-workspace/cobins/llfsm -I"/home/CORPUSERS/28848747/eclipse-workspace/cobins" -O0 -g3 -Wall -c -fmessage-length=0  -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


