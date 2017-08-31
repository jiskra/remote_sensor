################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: Copy\ of\ remote_sensor_gateway

# Tool invocations
Copy\ of\ remote_sensor_gateway: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross GCC Linker'
	mipsel-openwrt-linux-gcc -L/home/lvchenyang/mtk7688/toolchain/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2/lib -Xlinker -rpath /usr/lib -Xlinker -rpath /lib -o "Copy of remote_sensor_gateway" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) "Copy of remote_sensor_gateway"
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
