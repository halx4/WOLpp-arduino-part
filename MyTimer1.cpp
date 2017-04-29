#include "MyTimer1.h"


MyTimer1::MyTimer1(){
    enabled=false;
    mode=ONETIME;
    //Serial.println("MyTimer1 CONSTRUCTOR");
}        
//----------------------------
void MyTimer1::setEnabled(bool en){
        mark=millis();
        enabled=en;
}
//----------------------------
void MyTimer1::setInterval(unsigned long milliSeconds){
        interval=milliSeconds;

}
//----------------------------
void MyTimer1::setIntervalSeconds(unsigned long seconds){
        interval=seconds*1000;

}
//----------------------------
void MyTimer1::setIntervalMinutes(unsigned long minutes){
        //interval=minutes*1000;  //ONLY FOR DEBUGGING  . COMMENT IT AND UNCOMMENT LINE BELOW ##################################################################
        interval=minutes*60000;
}
//----------------------------
void MyTimer1::setCallback(timer_callback f){
        callback=f;

}
//----------------------------
void MyTimer1::setMode(TIMER_MODE newMode){
        mode=newMode;

}
//----------------------------
bool MyTimer1::isEnabled(){
        return enabled;

}
//----------------------------
void MyTimer1::check(){
        if(enabled){      
            if(millis() - mark >= interval ){
                    
                    if(mode==ONETIME)enabled=false;
                    else if (mode==REPEATING)setEnabled(true);   //restart timer
                    (*callback)();
            }                          
                
        }
}
//----------------------------



