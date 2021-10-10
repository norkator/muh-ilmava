# MUH Ilmava

MUH Ilmava Arduino temperature and humidity sensing hacks, 
which idea is to learn from gathering temperature and humidity data how this unit works.
Another purpose is to see how much this one heat transfer cell can gather heat and how much is dumped to `R4` channel.  
This repository code could possibly get some more "features" based on findings and learning curve.

MUH Ilmava is a house ventilation system and this specific unit is very old one from ~1989.



Sensor placing
============
Temperature|humidity sensors labeled as R1, R2, R3, R4 are placed in following way.  
`R1` - Incoming outside air.  
`R2` - Outgoing air to rooms.  
`R3` - Returning air from rooms.  
`R4` - Waste air outside house.


![ilmava_sensor_placing](./ilmava.jpg)

![air_channels](./channels.png)



Schematic
============
![schematic_bb](./schematic_bb.png) 



Backend
============
Is located at `/server` folder. It's a simple express backend which takes data and stores it into PostgreSQL database. 



Plotting
============
For plotting data I used `Superset`. I run it locally with Docker as their documentation instructs.

![superset](./superset.png) 

![plotting](./plotting.png) 
