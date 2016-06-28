################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/main.cpp \
../src/packet.cpp \
../src/server.cpp \
../src/session.cpp \
../src/utilities.cpp 

OBJS += \
./src/main.o \
./src/packet.o \
./src/server.o \
./src/session.o \
./src/utilities.o 

CPP_DEPS += \
./src/main.d \
./src/packet.d \
./src/server.d \
./src/session.d \
./src/utilities.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -std=c++1y -I../include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


