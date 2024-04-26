#--------------------------------------Makefile-------------------------------------

CFILES = $(wildcard ./src/*.c)
OFILES = $(CFILES:./src/%.c=./build/%.o)
GCCFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib

all: clean uart1_build printf_build cli_build kernel8.img run1
uart1: clean uart1_build kernel8.img run1
uart0: clean uart0_build kernel8.img run0

cli_build: ./cli/cli.c
	aarch64-linux-gnu-gcc $(GCCFLAGS) -c ./cli/cli.c -o ./build/cli.o

printf_build: ./printf/printf.c
	aarch64-linux-gnu-gcc $(GCCFLAGS) -c ./printf/printf.c -o ./build/printf.o

uart1_build: ./uart/uart1.c
	aarch64-linux-gnu-gcc $(GCCFLAGS) -c ./uart/uart1.c -o ./build/uart.o

uart0_build: ./uart/uart0.c
	aarch64-linux-gnu-gcc $(GCCFLAGS) -c ./uart/uart0.c -o ./build/uart.o

./build/boot.o: ./src/boot.S
	aarch64-linux-gnu-gcc $(GCCFLAGS) -c ./src/boot.S -o ./build/boot.o

./build/%.o: ./src/%.c
	aarch64-linux-gnu-gcc $(GCCFLAGS) -c $< -o $@

kernel8.img: ./build/boot.o ./build/uart.o ./build/printf.o ./build/cli.o $(OFILES)
	aarch64-linux-gnu-ld -nostdlib $^ -T ./src/link.ld -o ./build/kernel8.elf
	aarch64-linux-gnu-objcopy -O binary ./build/kernel8.elf kernel8.img

clean:
	rm -rf ./build/kernel8.elf ./build/*.o *.img

# Run emulation with QEMU
run1: 
	qemu-system-aarch64 -M raspi3b -kernel kernel8.img -serial null -serial stdio



run0: 
	qemu-system-aarch64 -M raspi3b -kernel kernel8.img -serial stdio
