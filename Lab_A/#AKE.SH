#!/bin/sh
nasm -f bin bootloader.asm -o bootloader.bin -l bootloader.list
gcc -c main.c
gcc -c ports.c
gcc -c video.c
ld -e main -Ttext 0x600 --section-start .bss=0x1500 --section-start .data=0x2000 \
--section-start .rodata=0x2100 -o kernel.o main.o video.o ports.o
objcopy -R .note -R .comment -S -O binary kernel.o kernel.bin

rm boot.img
dd if=/dev/zero of=boot.img bs=512 count=15
dd if=bootloader.bin of=boot.img bs=512 count=1
dd if=kernel.bin of=boot.img bs=512 count=14 seek=1