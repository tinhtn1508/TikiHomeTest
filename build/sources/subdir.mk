################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../sources/main.cc \
../sources/ParsingTextFile.cc \
../sources/SearchHandler.cc \
../sources/BM25.cc 

OBJS += \
./sources/main.o \
./sources/ParsingTextFile.o \
./sources/SearchHandler.o \
./sources/BM25.o

CPP_DEPS += \
./sources/main.d \
./sources/BM25.d \
./sources/ParsingTextFile.d \
./sources/SearchHandler.d \
./sources/./sources/main.d .d 

# Each subdirectory must supply rules for building sources it contributes
sources/%.o: ../sources/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<" -std=c++11 -pthread
# 	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<" -std=c++11 -pthread -D__BM25__
	@echo ' '


