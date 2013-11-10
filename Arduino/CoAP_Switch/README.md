 
## CoAP_Switch

CoAP application that allows controlling multiple CoAP enabled devices using an LCD Screen.
Device auto-detects the nearby CoAP devices and displays the offered resources in the screen.
Switching between the resources is possible using the UP and DOWN LCD screen buttons. 
Controlling actuators is possible via the LCD screen's LEFT and RIGHT buttons.


### Circuit


    +-------------+
    + LCD  SHIELD +
    +-------------+
    + Xbee SHIELD +
    +-------------+
    +   ARDUINO   +
    +-------------+


### Libraries
1. [Arduino-CoAP](https://github.com/dgiannakop/Arduino-CoAP)
1. [Xbee]()
1. [XbeeRadio]()
1. [XbeeRouting]()
1. [LiquidCrystal]()
