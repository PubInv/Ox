# RibbonFish Proof-of-Concept

Using the principle of Agile development, we are building a quick-dirty proof-of-concept codenamed RibbonFish.

The RibbonFish is a low-temperature (less than 100C) model consisting of two heating cloths and 12V fan, controlled with three simple thermocouples.
Although weak compared to the goal-product, this allows us to exercise much of the functionality that we will
eventually need.

## Hardware Choices

Because the RibbonFish is a real machine, it forces us to make hardware pin choices for the Arduino to
control the 6 hardware electronic devices. These choices are documented here to allow duplication
of the device; the hardware choices (GPIO pins) must match the software configuration.

1. The thermocouples are DS18B20 Temperatures sensors purchased from Adafruit, and configured using the [OneWire digital
protocol](https://learn.adafruit.com/adafruits-raspberry-pi-lesson-11-ds18b20-temperature-sensing/ds18b20) We use Digital Pin 2 as the signal pin for this.
3. The Fan and Heaters #1 and #2  are controlled via PWM (ranging from 0-12V.) The pins are: Fan = PIN 3, Heater #1 (Preheater) = PIN 4, Heater #2 (stack simulation) = PIN 5

The Fan is a simple DC 12 Vote Fan that fits inside a 4" PVC pipe. The heaters are Adafruit 4" x 5"  heating cloths, around 1 ohm each.

Note: We do not believe an unfiltered PWM signal is appropriate for the actual chemical stacks due to internal chemistry,
or at least this has not yet been tested. However, as a prelude to a real system, PWM can be used with the heating cloth
using simple power transistors.

## Photo of the RibbonFish

The Ribbon Fish consists of a PVC vent fan (12V, inline) connected via a heavy paper cone to piece of 3/4" PVC piping.
There are 3 such pipe sgements, each about 16mm long. These each have a 1/4" hole for the temperature sensor in the middle.
In the two joints between the three pipes, a heating cloth is rolled up into a cylinder and inserted into both pipes.
The leads are exposed to power the heating cloths.

![IMG-2837](https://user-images.githubusercontent.com/5296671/179431945-5230cd6c-c835-47c4-acae-b7c6fa9ac5d0.JPG)

## Libraries

PlatformIO provides useful package and configuration management, but
it still requires thought and documentation.

We believe the DS18B20 thermometers require these libraries

> pio lib install paulstoffregen/OneWire
> pio lib install milesburton/DallasTemperature


## Simple Commands for the RibbonFish

In order to continue moving to more verisimilitude, we need to implement a serial-port based
command structure within the RibbonFish.  Without being ready to specify everything,
let me describe a few commands.

### Parameter Commands

The first commands for the Ribbon Fish are:

1. W - Warmup
2. C - Cool Down
3. Set exhaust temperature than (first or second) heater
4. Set the speed of the fan

The command set the target temperatures for the first and second heater
```JavaScript
{ "com": "T",
  "par": 0,
  "mod": "T",
  "val" : 34.3
  }
```

```JavaScript
{ "com": "T",
  "par": 1,
  "mod": "T",
  "val" : 36.0
  }
```

This command says enter the cool down mode:
```JavaScript
{ "com": "C",
  "par": 0,
  "mod": "U",
  "val" : 0.0
  }
```

This command says enter the warm up mode:
```JavaScript
{ "com": "W",
  "par": 0,
  "mod": "U",
  "val" : 0.0
  }
```
