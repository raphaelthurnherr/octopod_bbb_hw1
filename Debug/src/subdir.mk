################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/BBBiolib.c \
../src/BBBiolib_ADCTSC.c \
../src/BBBiolib_McSPI.c \
../src/BBBiolib_PWMSS.c \
../src/loadconfig.c \
../src/manager.c \
../src/serial.c \
../src/tcpserver.c \
../src/th1_hwctrl.c \
../src/th2_core.c \
../src/th3_console.c \
../src/th4_tcp.c \
../src/th5_ai.c \
../src/th6_timers.c \
../src/th7_motion.c \
../src/tools.c 

OBJS += \
./src/BBBiolib.o \
./src/BBBiolib_ADCTSC.o \
./src/BBBiolib_McSPI.o \
./src/BBBiolib_PWMSS.o \
./src/loadconfig.o \
./src/manager.o \
./src/serial.o \
./src/tcpserver.o \
./src/th1_hwctrl.o \
./src/th2_core.o \
./src/th3_console.o \
./src/th4_tcp.o \
./src/th5_ai.o \
./src/th6_timers.o \
./src/th7_motion.o \
./src/tools.o 

C_DEPS += \
./src/BBBiolib.d \
./src/BBBiolib_ADCTSC.d \
./src/BBBiolib_McSPI.d \
./src/BBBiolib_PWMSS.d \
./src/loadconfig.d \
./src/manager.d \
./src/serial.d \
./src/tcpserver.d \
./src/th1_hwctrl.d \
./src/th2_core.d \
./src/th3_console.d \
./src/th4_tcp.d \
./src/th5_ai.d \
./src/th6_timers.d \
./src/th7_motion.d \
./src/tools.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc-4.8 -O0 -g3 -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


