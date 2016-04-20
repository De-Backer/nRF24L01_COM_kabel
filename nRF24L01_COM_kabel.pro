TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

# AVR info
DEVICE     = atmega8535
DEFINES += __AVR_ATmega8535__
CLOCK      = 8000000
#PROGRAMMER = "ponyser -P /dev/ttyS0 -V"
PROGRAMMER = "dragon_isp -P usb 03eb:2107 -v"
#PROGRAMMER = "dragon_pp -P usb 03eb:2107 -v"
FUSES      = -U hfuse:w:0xC9:m -U lfuse:w:0x8E:m

NAME = main

QMAKE_CC = avr-gcc
QMAKE_CXX = avr-g++
QMAKE_LINK = avr-g++
QMAKE_CFLAGS = -Wall -Os -Iusbdrv -mmcu=$${DEVICE}

# Customize the -mmcu flag

QMAKE_LFLAGS = -Os -Wl,--gc-sections -mmcu=$${DEVICE} -lm

QMAKE_EXTRA_TARGETS += eep hex flash

# Add any library paths needed to build your project

INCLUDEPATH =/opt/cross/avr/avr/include/

HEADERS =   nRF24L01.h \
    SPI.h \
    USART.h \
    device.h \
    RF24L01.h

SOURCES += main.c \
    SPI.c \
    USART.c \
    RF24L01.c



TARGET = $${NAME}.elf

QMAKE_CLEAN += $${NAME}.elf

eep.target = $${NAME}.eep

QMAKE_CLEAN += $${NAME}.eep

eep.commands = avr-objcopy -O ihex -j .eeprom

eep.commands += --set-section-flags=.eeprom=alloc,load --no-change-warnings

eep.commands += --change-section-lma .eeprom=0 $$TARGET $$eep.target

eep.depends = $$TARGET


hex.target = $${NAME}.hex

QMAKE_CLEAN += $${NAME}.hex

hex.commands = avr-objcopy -O ihex -R .eeprom $$TARGET $$hex.target

hex.depends = $${NAME}.eep

# make flash target to build and flash

flash.target = flash

# Customize the -p flag

flash.commands = avrdude -p $${DEVICE} -c $${PROGRAMMER}

flash.commands += -U flash:w:$${NAME}.hex

flash.depends = $${NAME}.hex

DISTFILES += \
    README.md
