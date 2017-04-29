#ifndef WOLPP_H
#define WOLPP_H

    //#include <Ethernet.h>
    //#include <HttpClient.h>
    #include <ESP8266WiFi.h>
    #include "ThingSpeak.h"
    #include "MyTimer1.h"

    const boolean ledOnState=LOW;
    const boolean ledOffState=!ledOnState;

    const boolean switchPressedState=HIGH;
    const boolean switchReleasedState=!switchPressedState;

    enum SERVER_STATE {SERVER_IDLE, WAITING_FOR_ACTION_COMPLETE, WAITING_FOR_WRITE_ALLOW};
    
    enum COMMAND {CMD_IDLE=0,PWR_MOMENTARY_PUSH,PWR_LONG_PUSH,RST_MOMENTARY_PUSH };
    
    enum LED_INDICATION {OFF=0,ON};
    enum BUTTON {POWER=0,RESET};
    enum BUTTON_ACTION {PRESS,RELEASE};
    enum FIELD { COMMAND_FIELD=1,
                 POWER_LED
      };

#endif
