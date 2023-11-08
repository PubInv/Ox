design option for a buck style current controlled low cost programmable power supply
Inbox

Lawrence Kincheloe 
Tue, Oct 24, 12:58 AM


The idea is that we could use a standard AC-DC power supply to provide constant voltage DC from 110 to 208v and then use a buck converter configured for constant current to allow for programmable power delivery to the stack.

Meanwell LRS-600-12, retails for $72.15
https://www.mouser.com/ProductDetail/MEAN-WELL/LRS-600-12?qs=sGAEpiMZZMs2%252BVrH5rwl1mv1K2Mgl3Xml3tU1hYC53vMx7R8wX7TFA%3D%3D

The  MAX20096 is a  Dual-Channel Synchronous Buck, High-Brightness LED Controller, which means it can control TWO stacks, and because it has external mosfets, we can size the power output fed by a single high current constant voltage power supply. 

https://www.mouser.com/ProductDetail/Analog-Devices-Maxim-Integrated/MAX20096ATJ-VY%2b?qs=lc2O%252BfHJPVZbD9Fi3rGrOw%3D%3D
https://www.mouser.com/datasheet/2/609/MAX20096_MAX20097-3127961.pdf

The nice thing about this topology is that since it's synchronous so efficiency is high, and it can sink current, which may allow for monitoring the voltage potential of the partial pressure oxygen differential between the stack high and low pressure sides.

A few issues, the inductor is going to be the potentially most difficult part to source, in extreme cases, it may need to be custom wound. 

The SPI current variable is only 6 bits, which I assume means there are only 64 current steps, which may or may not be enough resolution. An alternative would be to use the Analog Dimming Input, which may allow for higher external resolution. 

The controller would likely be a separate MCU, possibly an arduino, and would use the same pin connection as the existing power supply. 

