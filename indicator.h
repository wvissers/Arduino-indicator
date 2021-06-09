/*
* Author: Wim Vissers
* The purpose of this library is to use a single LED as indicator.
*/
#ifndef indicator_h
#define indicator_h

#include <Arduino.h>

#ifdef ESP8266
#include <functional>
#define STATE_CALLBACK_SIGNATURE std::function<void(byte, byte, byte)> callback
#else
#define STATE_CALLBACK_SIGNATURE void (*callback)(byte, byte, byte)
#endif


struct LEDProperties {
    byte pin;
    byte idleState;
    boolean analog;
    int brightness;
};

class Step {
private:
    Step* _next;
    unsigned long _duration;
    byte _state;
public:
    Step(const unsigned long duration, const byte state);
    Step *getNext();
    void setNext(Step* next);  
    long getDuration(); 
    byte getState();
};

class LEDMode {
private:
    LEDMode* _previous;
    Step* _first;
    byte _id;
    unsigned long _last;
    byte _theState;
protected:
    Step* _current;
public:
    LEDMode(LEDMode *previous, const byte id);
    LEDMode(LEDMode *previous, const byte idleState, const byte id, const unsigned long msOn, const unsigned long msOff);
    LEDMode* getPrevious();
    void addStep(Step* step);
    void reset(const unsigned long now);
    virtual boolean execute(LEDProperties* led, const unsigned long now, STATE_CALLBACK_SIGNATURE);
    byte getId();
};

/**
 * Usage:
 * 
 * Create: Indicator indicator(LED_PIN);
 * 
 * Configure: add at leat 1 mode. A mode can be specified as
 * pattern, with . or S meaning short, and - or L meaning long.
 * Please note that pattern starts with LED on, next step is 
 * LED off, etc.
 * 
 * A mode can also be specified as a sequence, by an array 
 * of longs.
 */ 
class Indicator {
private:
   LEDMode* _ledModes;
   LEDMode* _currentMode;
   LEDProperties _led;
   unsigned long _last;
   boolean _enabled;
   STATE_CALLBACK_SIGNATURE;
public:
   Indicator(const byte ledPin);
   Indicator(const byte ledPin, const byte idleState);
   Indicator(const byte ledPin, const byte idleState, STATE_CALLBACK_SIGNATURE);
   void setCallback(STATE_CALLBACK_SIGNATURE);
   boolean begin(); 
   boolean addMode(LEDMode* mode);
   boolean addMode(const byte id, boolean on);
   boolean addMode(const byte id, const unsigned long msShort, const unsigned long msLong, char* pattern);
   boolean addMode(const byte id, int size, const unsigned long sequence[]);
   boolean addMode(const byte id, const unsigned long msOn, const unsigned long msOff);
   boolean loop();
   boolean setMode(const byte id);
   void setEnabled(const boolean enabled);
   void setBrightness(int brightness);
   boolean isEnabled();
   byte getMode();
   LEDMode* getMode(const byte id);
};


#endif
