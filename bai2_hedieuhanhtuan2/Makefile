.RECIPEPREFIX = >

TARGET  = bai2
BUILD   = build
CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE    = arm-none-eabi-size

RTOS    = FreeRTOS-Kernel
HAL     = stm32f1xx_hal_driver
CMSIS_D = cmsis_device_f1
CMSIS_C = cmsis_core

SRCS = main.c startup.c \
       $(CMSIS_D)/Source/Templates/system_stm32f1xx.c \
       $(HAL)/Src/stm32f1xx_hal.c \
       $(HAL)/Src/stm32f1xx_hal_cortex.c \
       $(HAL)/Src/stm32f1xx_hal_gpio.c \
       $(HAL)/Src/stm32f1xx_hal_rcc.c \
       $(HAL)/Src/stm32f1xx_hal_dma.c \
       $(HAL)/Src/stm32f1xx_hal_uart.c \
       $(RTOS)/tasks.c \
       $(RTOS)/queue.c \
       $(RTOS)/list.c \
       $(RTOS)/portable/GCC/ARM_CM3/port.c \
       $(RTOS)/portable/MemMang/heap_4.c

OBJS = $(addprefix $(BUILD)/,$(SRCS:.c=.o))

INCS = -I. \
       -I$(RTOS)/include \
       -I$(RTOS)/portable/GCC/ARM_CM3 \
       -I$(HAL)/Inc \
       -I$(CMSIS_D)/Include \
       -I$(CMSIS_C)/CMSIS/Core/Include

DEFS    = -DSTM32F103xB -DUSE_HAL_DRIVER
CFLAGS  = -mcpu=cortex-m3 -mthumb -Os -g -Wall -ffunction-sections -fdata-sections -MMD -MP $(DEFS) $(INCS)
LDFLAGS = -mcpu=cortex-m3 -mthumb -T linker.ld -nostartfiles -Wl,--gc-sections \
          -specs=nano.specs -specs=nosys.specs -Wl,-Map=$(BUILD)/$(TARGET).map

all: $(BUILD)/$(TARGET).bin

$(BUILD)/$(TARGET).elf: $(OBJS)
> $(CC) $(OBJS) $(LDFLAGS) -o $@
> $(SIZE) $@

$(BUILD)/%.bin: $(BUILD)/%.elf
> $(OBJCOPY) -O binary $< $@

$(BUILD)/%.o: %.c
> mkdir -p $(dir $@)
> $(CC) $(CFLAGS) -c $< -o $@

flash: all
> st-flash write $(BUILD)/$(TARGET).bin 0x08000000

clean:
> rm -rf $(BUILD)

.PHONY: all flash clean
-include $(OBJS:.o=.d)
