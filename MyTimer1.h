#ifndef MYTIMER1_H
#define MYTIMER1_H

#include <Arduino.h>
//#include "OutputsHandler.h"

class OutputsHandler;

typedef void (*timer_callback)(void);
enum TIMER_MODE { ONETIME=0, REPEATING};

class MyTimer1 {
    public:
        // constructor
        MyTimer1(); 
        
        void setEnabled(bool en);
        
        void setInterval(unsigned long milliSeconds);
                
        void setIntervalSeconds(unsigned long seconds);
        
        void setIntervalMinutes(unsigned long minutes);
        
        void setCallback(timer_callback f);
        
        void setMode(TIMER_MODE mode);
        
        bool isEnabled();
        
        // this function must be called inside loop()
        void check();
        
    private:

        // value returned by the millis() function
        // in the previous run() call
        volatile unsigned long mark;
    
        // pointers to the callback functions
        timer_callback callback;
    
        unsigned long interval;
        
        volatile bool enabled;
        
        TIMER_MODE mode;
    
};

#endif
