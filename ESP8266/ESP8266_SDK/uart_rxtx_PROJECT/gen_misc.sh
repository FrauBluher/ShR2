#!/bin/bash -x
make

rm -f ./bin/eagle.app.v6.flash.bin ./bin/eagle.app.v6.irom0text.bin

cd .output/eagle/debug/image

esptool -eo eagle.app.v6.out -bo eagle.app.v6.flash.bin -bs .text -bs .data -bs .rodata -bc -ec
xtensa-lx106-elf-objcopy --only-section .irom0.text -O binary eagle.app.v6.out eagle.app.v6.irom0text.bin
cp eagle.app.v6.flash.bin ../../../../bin/
cp eagle.app.v6.irom0text.bin ../../../../bin/

cd ../../../../

