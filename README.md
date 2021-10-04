# MUH Ilmava

MUH Ilmava Arduino temperature sensing hacks.


Sensor placing
============
Three temperature sensors labeled as R1, R2, R3 are placed in following way.  
`R1` - Incoming outside air.  
`R2` - Outgoing air to rooms.  
`R3` - Returning air from rooms.  
`R4` - After heating coil (used to heat air to avoid heat recovery cell to freeze).
  

(Image was taken before filter change and cleaning of unit)
![ilmava_sensor_placing](./ilmava.jpg)

![air_channels](./channels.png)


Schematic
============
![schematic_bb](./schematic_bb.png) 



OLED screen notes
============

Module
-----
SSD1306 OLED

Pinouts
-----
* SDA -> A4 (i2c)
* SCL -> A5 (i2c)
* GND -> Ground
* VDD -> 5v (Screen pin labeled as VCC or VDD)

Required libraries
-----
* Adafruit BusIO
* Adafruit-GFX-Library
* Adafruit_SSD1306
