hex:
	avr-gcc -Os -DF_CPU=8000000L -DBAUD=19200 -mmcu=atmega644pa -o brainfuck.elf brainfuck.c
	avr-objcopy -O ihex brainfuck.elf brainfuck.hex

clean:
	rm -fv brainfuck.hex brainfuck.elf

flash:
	avrdude -p m644p -b 115200 -c buspirate -V -P /dev/ttyUSB0 -U flash:w:brainfuck.hex
