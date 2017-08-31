################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Fonts.c \
../SSD1306.c \
../output.c \
../read_comment.c \
../remote_sensor_gateway.c 

OBJS += \
./Fonts.o \
./SSD1306.o \
./output.o \
./read_comment.o \
./remote_sensor_gateway.o 

C_DEPS += \
./Fonts.d \
./SSD1306.d \
./output.d \
./read_comment.d \
./remote_sensor_gateway.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


