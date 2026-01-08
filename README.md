# FurModularity
Smart watch/audio mixer/module hub/game console/whatever
The FurModularity is a """watch""" meant "For Modularity", it's based on a ESP32-S3 as it's brain

It has 5 input/output connectors for external modules, these modules can be anything, be it a flashlight, a laser pointer, SD card, a second screen, or anything you can imagine, these modules interface with the main board through a SPI bus, a microcontroller inside the module card must interface between the actual module device and the ESP32, this allows more flexibility since a driver on the watch and custom code on the slave microcontroller allows for way more customization than directly interfacing the SPI bus with the module device, it would result in a way more limited module selection, or require you to manually select what kind of module is connected on the user inteface, which would ruin the hotplugging since you'd need to open settings every time you connect something new.

It also has a two input and two output audio mixer, via software you can route inputs 1, 2 or both to outputs 1, 2 or both, the routing is done by two 74VHC4066M Analog Switch ICs, one for each input channel, and you can also change the input volumes, that is done by two MCP4261 ICs, one for each channel too.

I originally built this project as a curious kid exploring CAD software, and I always liked electronics, I wanted to create my own stuff, but I never had the money for it. Now with more experience I restarted this project from scratch hoping to get funds for it.

This is how the PCB looks like, the back side has no components, thus simplifying assembly:
![Board image](/Images/PCB/PCB_Final_3D.jpg)
![Board routing](/Images/PCB/PCB_CAD.png)

This is how the case looks like:
![Case](/Images/Case/Case_Top.png)
![Case2](/Images/Case/Case_45.png)

And finally, here is the schematics:
This is the power circuit, it handles battery discharging, PowerPath, and generating 5V and 3.3V rails
![Power circuit](/Images/Schematic/Pwr.png)

This is the digital circuit, it shows connections from the MCU to other components in the board
![Digital circuit](/Images/Schematic/MCU.png)

This is the audio circuit, it handles audio routing and volume control, everything is controlled by the MCU
![Audio circuit](/Images/Schematic/Audio.png)
