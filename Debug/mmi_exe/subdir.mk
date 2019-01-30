################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../mmi_exe/AppDelegate.cpp \
../mmi_exe/main.cpp 

OBJS += \
./mmi_exe/AppDelegate.o \
./mmi_exe/main.o 

CPP_DEPS += \
./mmi_exe/AppDelegate.d \
./mmi_exe/main.d 


# Each subdirectory must supply rules for building sources it contributes
mmi_exe/%.o: ../mmi_exe/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/home/CORPUSERS/28848747/eclipse-workspace/cobins/llfsm -I"/home/CORPUSERS/28848747/eclipse-workspace/cobins" -O0 -g3 -Wall -c -fmessage-length=0  -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


