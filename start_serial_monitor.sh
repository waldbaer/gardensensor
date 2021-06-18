#!/bin/bash
set -e

# -- Config --
serial_device="/dev/ttyUSB0"
serial_baudrate=9600

# -- Run --
echo "Configure serial device '${serial_device}' to baudrate '${serial_baudrate}'"
stty -F ${serial_device} ${serial_baudrate} cs8 cread clocal

echo "---- serial monitor ----"
cat ${serial_device}
