# Makefile for timing wrapper system example
# Can be adapted for your specific STM32 toolchain

# Compiler
CC = arm-none-eabi-gcc

# Compiler flags for ARM Cortex-M (adjust for your specific MCU)
# Example for STM32F4 (Cortex-M4)
CFLAGS = -mcpu=cortex-m4 \
         -mthumb \
         -mfloat-abi=soft \
         -Wall \
         -Wextra \
         -Wpedantic \
         -std=c99 \
         -O2

# Source files
SOURCES = timing_wrapper_system.c \
          port.c \
          example_usage.c

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Output binary
TARGET = timing_example.elf

# Default target
all: $(TARGET)

# Link object files
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile source files
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(TARGET)

# Phony targets
.PHONY: all clean

# Dependencies
timing_wrapper_system.o: timing_wrapper_system.c timing_wrapper_system.h port.h
port.o: port.c port.h
example_usage.o: example_usage.c timing_wrapper_system.h port.h
