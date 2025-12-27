# FurModularity
Smart watch/audio mixer/module hub/game console/whatever
The FurModularity is a """watch""" meant "For Modularity", it's based on a ESP32-S3 as it's brain

It has 5 input/output connectors for external modules, these modules can be anything, be it a flashlight, a laser pointer, SD card, a second screen, or anything you can imagine, these modules interface with the main board through a SPI bus, a microcontroller inside the module card must interface between the actual module device and the ESP32, this allows more flexibility since a driver on the watch and custom code on the slave microcontroller allows for way more customization than directly interfacing the SPI bus with the module device, it would result in a way more limited module selection, or require you to manually select what kind of module is connected on the user inteface, which would ruin the hotplugging since you'd need to open settings every time you connect something new.

It also has a two input and two output audio mixer, via software you can route inputs 1, 2 or both to outputs 1, 2 or both, the routing is done by two 74VHC4066M Analog Switch ICs, one for each input channel, and you can also change the input volumes, that is done by two MCP4261 ICs, one for each channel too.
