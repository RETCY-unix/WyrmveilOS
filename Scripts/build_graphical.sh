#!/bin/bash

echo "Building SEPPUKU OS GRAPHICAL v2.0..."

if [ -d "../boot" ]; then
    cd ..
fi

mkdir -p build

echo "[1/11] Assembling VESA bootloader..."
nasm -f bin boot/boot_vesa.asm -o build/boot.bin

echo "[2/11] Assembling IDT handlers..."
nasm -f elf32 Kernel/idt.asm -o build/idt_asm.o

echo "[3/11] Compiling graphics driver..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/drivers/graphics.c -o build/graphics.o

echo "[4/11] Compiling terminal emulator..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/drivers/terminal.c -o build/terminal.o

echo "[5/11] Compiling keyboard driver..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/drivers/keyboard.c -o build/keyboard.o

echo "[6/11] Compiling graphical shell..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c user/shell/shell_graphical.c -o build/shell_graphical.o

echo "[7/11] Compiling ISR handler..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/isr.c -o build/isr.o

echo "[8/11] Compiling IDT..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/idt.c -o build/idt.o

echo "[9/11] Compiling kernel..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/kernel_graphical.c -o build/kernel.o

echo "[10/11] Linking kernel..."
ld -m elf_i386 -Ttext 0x1000 --oformat binary \
   -e kernel_main \
   build/kernel.o build/graphics.o build/terminal.o build/keyboard.o \
   build/shell_graphical.o build/idt.o build/isr.o build/idt_asm.o \
   -o build/kernel.bin

echo "[11/11] Creating disk image..."
dd if=/dev/zero of=build/os.img bs=512 count=2880 2>/dev/null
dd if=build/boot.bin of=build/os.
