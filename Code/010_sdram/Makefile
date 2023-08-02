objs = start.o 
objs += main.o

objs += led/led.o
objs += uart/uart.o
objs += nor/nor.o
objs += sdram/sdram.o

all: $(objs)
	arm-linux-ld -T sdram.lds $^ -o sdram.elf
	arm-linux-objcopy -O binary -S sdram.elf sdram.bin
	arm-linux-objdump -D sdram.elf > sdram.dis
clean:
	rm -f *.bin $(objs) *.elf *.dis
	
%.o : %.c
	arm-linux-gcc -march=armv4 -c -o $@ $<

%.o : %.S
	arm-linux-gcc -march=armv4 -c -o $@ $<
	
