[![MIT License](https://img.shields.io/github/license/waldbaer/gardensensor?style=flat-square)](https://opensource.org/licenses/MIT)
[![GitHub issues open](https://img.shields.io/github/issues/waldbaer/gardensensor?style=flat-square)](https://github.com/waldbaer/gardensensor/issues)


# KNX Garden Sensor #
KNX control unit to read garden sensor values.


## Key features ##
Supported sensors / inputs:
 - 1x 4-20mA current loop input (e.g. TL-136 water pressure sensor for cistern fill level)
 - 3x 16bit analog inputs (e.g. for Truebner SMT 50 humidity / temperature sensor)
 - 1x 10bit analog input (e.g. for Truebner SMT 50 humidity / temperature sensor)

## Requirements ##
 - Arduino pro mini (3,3V or 5V)
 - Siemens BCU (KNX bus coupling unit)
 - Some soldering skills
 - [Optional] Fits best into Phoenix Contact EH 45 FLAT DEV-KIT KMGY case

## Setup ##
```
# Compile and flash to Arduino
./build-and-flash
```

## Usage ##

1. Connect sensor devices
2. Connect to KNX network

Current sensor values are transmitted periodically to pre-configured group addresses.
