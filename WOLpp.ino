#include "WOLpp.h"
#include "PersonalInfo.h"
//------ CONSTANTS ----------------------------
const uint8_t LED_PIN=D1;
const uint8_t PWR_SWITCH_PIN=D5;
const uint8_t RST_SWITCH_PIN=D6;

const unsigned long CHECK_FOR_CMD_INTERVAL=5000; //ms
const unsigned long UPDATE_INDICATIONS_INTERVAL=10000; //ms
const unsigned long INTERVAL_FOR_NEXT_WRITE=18000;
const unsigned long MOMENTARY_PUSH_INTERVAL=800;
const unsigned long LONG_PUSH_INTERVAL=5000;

//-------- Networking STUFF -------------------------

WiFiClient  client;
int status = WL_IDLE_STATUS;

// MAC address for your Ethernet shield
byte mac[] = { 0x89, 0x4E, 0xD5, 0x61, 0x57, 0x12 };

//---------- VARIABLES ---------------------------------

MyTimer1 indicationsUpdateTimer,buttonPressTimer,nextWriteTimer,nextReadTimer;
SERVER_STATE serverState;
BUTTON activeButton;

//-------------------------------------------------------

void setup() {
    Serial.begin(9600);

    pinMode(PWR_SWITCH_PIN,OUTPUT);
    pinMode(RST_SWITCH_PIN,OUTPUT);
    digitalWrite(PWR_SWITCH_PIN,switchReleasedState);
    digitalWrite(RST_SWITCH_PIN,switchReleasedState);
    
    //------WIFI------
    WiFi.begin(ssid, password);
  
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    //-------ThingSpeak--------
    ThingSpeak.begin(client);

    //----- Initializations -----
    indicationsUpdateTimer.setCallback(indicationsUpdateTimerEvent);
    indicationsUpdateTimer.setInterval(UPDATE_INDICATIONS_INTERVAL);
    indicationsUpdateTimer.setMode(REPEATING);
    indicationsUpdateTimer.setEnabled(true);
    
    nextWriteTimer.setCallback(nextWriteTimerEvent);
    nextWriteTimer.setInterval(INTERVAL_FOR_NEXT_WRITE);

    nextReadTimer.setCallback(nextReadTimerEvent);
    nextReadTimer.setInterval(CHECK_FOR_CMD_INTERVAL);
    nextReadTimer.setMode(REPEATING);
    nextReadTimer.setEnabled(true);
    
    buttonPressTimer.setCallback(buttonPressTimerEvent);

    resetCommand();
    serverState=SERVER_IDLE;
    
    Serial.println("SETUP FINISHED");
}

//-----------------------------------------

void loop() {
    indicationsUpdateTimer.check();
    nextWriteTimer.check();
    buttonPressTimer.check();
    nextReadTimer.check();


}

//----------------------------------------------

void indicationsUpdateTimerEvent(){
  //Serial.println("CALLEDBACK!!indicationsUpdateTimerEvent");
  switch (serverState){
    case SERVER_IDLE:
      
      setRemoteIndications(getPowerLedState());
      break;
    case WAITING_FOR_ACTION_COMPLETE:
      
      break;
    case WAITING_FOR_WRITE_ALLOW:
      
      break;
  }
  
}

void nextWriteTimerEvent(){
  //Serial.println("CALLEDBACK!!nextWriteTimerEvent");
    switch (serverState){
    case SERVER_IDLE:
      
      break;
    case WAITING_FOR_ACTION_COMPLETE:
      
      break;
    case WAITING_FOR_WRITE_ALLOW:
      setCombined(CMD_IDLE,getPowerLedState());
      serverState=SERVER_IDLE;
      
      break;
  }
}

void buttonPressTimerEvent(){
  //Serial.println("CALLEDBACK!!buttonPressTimerEvent");
    switch (serverState){
    case SERVER_IDLE:
      
      break;
    case WAITING_FOR_ACTION_COMPLETE:
      buttonDo(activeButton,RELEASE);
      serverState=WAITING_FOR_WRITE_ALLOW;
      break;
    case WAITING_FOR_WRITE_ALLOW:
      
      break;
  }
}

void nextReadTimerEvent(){
  //Serial.println("!!nextReadTimerEvent");
  COMMAND cmd;
    switch (serverState){
    case SERVER_IDLE:
      cmd=readCurrentCommand();
      if(cmd!=CMD_IDLE)interpretCommand(cmd);
      break;
    case WAITING_FOR_ACTION_COMPLETE:
      
      break;
    case WAITING_FOR_WRITE_ALLOW:
      
      break;
  }
}

//-------------------

void resetCommand(){
    int response;
    COMMAND currentCommand=readCurrentCommand();
    if (currentCommand!=CMD_IDLE){
        response=setCurrentCommand(CMD_IDLE);
        //Serial.println("IF");
        while(response!=OK_SUCCESS){
            response=setCurrentCommand(CMD_IDLE);
            delay(1000);
            //Serial.println("WHILE");
        }
   }
}

//----------------------

void interpretCommand(COMMAND cmd){
  Serial.print("interpret ");
  Serial.println(cmd);
    switch(cmd){
      case PWR_MOMENTARY_PUSH:
        activeButton=POWER;
        buttonDo(activeButton,PRESS);
        buttonPressTimer.setInterval(MOMENTARY_PUSH_INTERVAL);
        buttonPressTimer.setEnabled(true);
        nextWriteTimer.setEnabled(true);
        break;
      case PWR_LONG_PUSH:
        activeButton=POWER;
        buttonDo(activeButton,PRESS);
        buttonPressTimer.setInterval(LONG_PUSH_INTERVAL);
        buttonPressTimer.setEnabled(true);
        nextWriteTimer.setEnabled(true);
        break;
      case RST_MOMENTARY_PUSH:
        activeButton=RESET;
        buttonDo(activeButton,PRESS);
        buttonPressTimer.setInterval(MOMENTARY_PUSH_INTERVAL);
        buttonPressTimer.setEnabled(true);
        nextWriteTimer.setEnabled(true);
      break;
    }
    serverState=WAITING_FOR_ACTION_COMPLETE;
}

//----------------------

COMMAND readCurrentCommand(){
  int com=ThingSpeak.readIntField(myChannelNumber, COMMAND_FIELD, myReadAPIKey);
  return static_cast<COMMAND>(com);
}

//----------------------

//returns HTTP response code
int setCurrentCommand(COMMAND com){
    return ThingSpeak.writeField(myChannelNumber, COMMAND_FIELD, com, myWriteAPIKey);
}

//---------------------
//updates thingspeak command and indications
//returns HTTP response code
int setCombined(COMMAND com,LED_INDICATION led){ 
    Serial.println("sending combined ");
    ThingSpeak.setField(COMMAND_FIELD,COMMAND2int(com));
    ThingSpeak.setField(POWER_LED,LED_INDICATION2int(led));
    return ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
}

//---------------------


void buttonDo(BUTTON btn,BUTTON_ACTION action){
  Serial.print("---------> buttonDo ");
  Serial.print(btn);
  Serial.print(" ");
  Serial.println(action);

uint8_t button;

  switch(btn){
    case POWER:
      button=PWR_SWITCH_PIN;
      break;
    case RESET:
      button=RST_SWITCH_PIN;
      break;
  }

  switch(action){
    case PRESS:
      digitalWrite(button,switchPressedState);
      break;
    case RELEASE:
      digitalWrite(button,switchReleasedState);
      break;
  }
}

//----------------------

LED_INDICATION getPowerLedState(){//TODO
  boolean reading=digitalRead(LED_PIN);
  if(reading==ledOnState)return ON;
  else return OFF;
}

//----------------------

void setRemoteIndications(LED_INDICATION powerLed){
    Serial.println("checking if remote indication is correct...");
    int ledFeedReadingAsInt=ThingSpeak.readIntField(myChannelNumber, POWER_LED, myReadAPIKey);
    LED_INDICATION ledFeedReading= static_cast<LED_INDICATION>(ledFeedReadingAsInt);
    if(ledFeedReading !=  powerLed){
      Serial.println("led indication feed do not match. updating...");
      ThingSpeak.writeField(myChannelNumber, POWER_LED, powerLed, myWriteAPIKey);
    }
}

//-----------------

int COMMAND2int(COMMAND com){
    switch(com){
      case CMD_IDLE:
      return 0;
      case PWR_MOMENTARY_PUSH:
      return 1;
      case PWR_LONG_PUSH:
      return 2;
      case RST_MOMENTARY_PUSH:
      return 3;
      default:
      return (-1);
    }
  }

//COMMAND int2COMMAND(int num){}

int LED_INDICATION2int(LED_INDICATION led){
    switch(led){
      case OFF:
      return 0;
      case ON:
      return 1;
      default:
      return (-1);
    }
  }
