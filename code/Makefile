SRCS=main.c ir_Samsung32.c usi.c
TARGET=IR_ATmega

#ATmega48A/PA/88A/PA/168A/PA/328/P
#MCU=atmega328p
#AVRDUDE_MCU=m328p
#F_CPU=16000000
#BAUD=115200
#PORT=/dev/ttyACM0
#ende ATmega

#ATtiny25/45/85
MCU=attiny45
AVRDUDE_MCU=ATtiny45
F_CPU=1000000
BAUD=9600
PORT=/dev/ttyUSB0
#ende ATtiny

PROGR=arduino
CC=avr-gcc
OBJCOPY=avr-objcopy

CFLAGS=-g -Os -mmcu=${MCU} -DF_CPU=${F_CPU} -I.
#nächste Zeile geht nicht mit asambler-Befehlen!
#CFLAGS+=-std=C99
CFLAGS+=-Wall
#wenns nervt, dann das nächste auskommentieren
#CFLAGS+=-Wno-unused
CFLAGS+=-Wextra

#fuse!!!!
LFUSE=
HFUSE=
EFUSE=

all:
	$(CC) ${CFLAGS} -o ${TARGET}.bin ${SRCS}
	${OBJCOPY} -j .text -j .data -O ihex ${TARGET}.bin ${TARGET}.hex	
	avrdude -c ${PROGR} -p ${AVRDUDE_MCU} -P ${PORT} -b ${BAUD} -U flash:w:${TARGET}.hex
	rm -f *.bin *.hex *.o

file:
	${CC} ${CFLAGS} -o ${TARGET}.bin ${SRCS}
	${OBJCOPY} -j .text -j .data -O ihex ${TARGET}.bin ${TARGET}.hex
	avr-size --format=avr --mcu=${MCU} ${TARGET}.bin
	rm -f *.bin *.hex *.o
	
asm:
	${CC} ${CFLAGS} -fno-asynchronous-unwind-tables -fno-exceptions -fno-rtti -fverbose-asm \
	tft.c > tft.s -S -o- | less
 
flash:
	avrdude -c ${PROGR} -p ${AVRDUDE_MCU} -P ${PORT} -b ${BAUD} -U flash:w:${TARGET}.hex

fuselesen:
	avrdude -c ${PROGR} -p ${AVRDUDE_MCU} -P ${PORT} -b ${BAUD} -U lfuse:r:-:h -U hfuse:r:-:h -U efuse:r:-:h
	
lfuseschreiben:
	avrdude -c ${PROGR} -p ${AVRDUDE_MCU} -P ${PORT} -b ${BAUD} -U lfuse:w:${LFUSE}:m
	
hfuseschreiben:
	avrdude -c ${PROGR} -p ${AVRDUDE_MCU} -P ${PORT} -b ${BAUD} -U hfuse:w:${HFUSE}:m

efuseschreiben:
	avrdude -c ${PROGR} -p ${AVRDUDE_MCU} -P ${PORT} -b ${BAUD} -U efuse:w:${EFUSE}:m
	
exe:
	gcc -Wall -o ${TARGET} ${SRCS}
	rm -f *.bin *.hex *.s *.o

clean:
	rm -f *.bin *.hex *.s *.lst *.atxt *.o
