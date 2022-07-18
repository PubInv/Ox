# RibbonFish Proof-of-Concept

Using the principle of Agile development, we are building a quick-dirty proof-of-concept codenamed RibbonFish.

The RibbonFish is a low-temperature (less than 100C) model consisting of two heating cloths and 12V fan, controlled with three simple thermocouples.
Although weak compared to the goal-product, this allows us to exercise much of the functionality that we will
eventually need.

## Hardware Choices

Because the RibbonFish is a real machine, it forces us to make hardware pin choices for the Arduino to
control the 6 hardware electronic devices. These choices are documented here to allow duplication
of the device; the hardware choices (GPIO pins) must match the software configuration.

1. The thermocouples are DS18B20 Temperatures sensors purchased from Adafruit, and configured using the [OneWire digial
protocal](https://learn.adafruit.com/adafruits-raspberry-pi-lesson-11-ds18b20-temperature-sensing/ds18b20) We use Digial Pin 2 as the signal pin for this.
3. The Fan and Heaters #1 and #2  are controlled via PWM (ranging from 0-12V.) The pins are: Fan = PIN 3, Heater #1 (Preheater) = PIN 4, Heater #2 (stack simulation) = PIN 5

The Fan is a simple DC 12 Vote Fan that fits inside a 4" PVC pipe. The heaters are Adafruit 4" x 5"  heating cloths, around 1 ohm each.

Note: We do not believe an unfiltered PWM signal is appropriate for the actual chemical stacks due to internal chemistry,
or at least this has not yet been tested. However, as a prelude to a real system, PWM can be used with the heating cloth
using simple power transistors.