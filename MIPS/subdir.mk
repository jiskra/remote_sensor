################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Fonts.c \
../SSD1306.c \
../output.c \
../read_comment.c \
../remote_sensor_gateway.c \
../sim900a.c 

OBJS += \
./Fonts.o \
./SSD1306.o \
./output.o \
./read_comment.o \
./remote_sensor_gateway.o \
./sim900a.o 

C_DEPS += \
./Fonts.d \
./SSD1306.d \
./output.d \
./read_comment.d \
./remote_sensor_gateway.d \
./sim900a.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	mipsel-openwrt-linux-gcc -DOLED -I/home/lvchenyang/mtk7688/toolchain/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


