#!/bin/bash
#Sets up IO and starts acquisition.

echo "Setting up SPI and transfer request pins..."
echo ""

#Compile device tree kernel with SPI1 for spidev
dtc -O dtb -o BB-SPI1-01-00A0.dtbo -b 0 -@ BB-SPI1-01-00A0.dts
cp BB-SPI1-01-00A0.dtbo /lib/firmware/


#Set up request pin from PIC to BBB.
#gpio3[19] Corresponds to pin 27 on P9
#gpio0[14] Corresponds to pin 26 on P9
#gpio3[19] corresponds to gpio115
#gpio0[14] corresponds to gpio14

echo "If GPIO already init this will throw an error, which is okay."
echo 115 > /sys/class/gpio/export
echo 14 > /sys/class/gpio/export
echo ""

echo "GPIO pin interactors"
ls -alH /sys/class/gpio/gpio115
echo in > /sys/class/gpio/gpio115/direction

echo "GPIO pin interactors"
ls -alH /sys/class/gpio/gpio14
echo out > /sys/class/gpio/gpio14/direction
echo ""

echo "SPI dev stream."
#Make sure that our spidev exists...
echo BB-SPI1-01 > /sys/devices/bone_capemgr.*/slots
ls -lh /dev/spi*

rm -rf AcquiredDataRaw
./DAQ-BBB
