/*
Example sketch to demonstrate typical use of the indicator library. This
sketch uses the builtin LED for the demonstration. 

Upload the sketch with the Arduino IDE or your usual IDE to a suitable 
board. After upload the LED should be blinking on and of indication a
morse SOS, ...---... 
   
*/
#include "indicator.h"

/**
 * Define the Indicator object. It uses the predefined constant BUILTIN_LED,
 * that most boards support. If not supported by your board, change this to
 * the pin the LED is connected to (like D1, D2 etc.).
 */
Indicator indicator(BUILTIN_LED, HIGH);

/**
 * Even though we have only one mode in this example, it is good practice 
 * to define mode constants.
 */ 
#define MODE_SOS 0x00

/**
 * Define the intervals in milliseconds for each step. E.g. 500 means set
 * LED 500 ms on, followed by 500 ms off.
 */
#define INTERVAL_SHORT 200
#define INTERVAL_LONG 500

void setup()
{
    // Initialize the serial port.
    Serial.begin(9600);
    Serial.print("Morse SOS is blinking");

    // Begin indicator operation. This also configures the port for the LED
    // as OUTPUT.
    indicator.begin();

    // Define the mode. In this example, there is a single mode, just sending
    // morse SOS (...---...) over and over again. The pattern is defined as a 
    // string, where 'S' indicates the short interval, and 'L' indicates the
    // long interval. For each step there is a time for LED on and one for LED
    // off. If you want e.g. a short blink of the LED, followed by a long pause,
    // the pattern would be "SL". To illustrate a more complex pattern, we have
    // the morse code here.
    indicator.addMode(MODE_SOS, INTERVAL_SHORT, INTERVAL_LONG, "SSSSSLLLLLLLSSSSSL");
 
    // Select initial mode.
    indicator.setMode(MODE_SOS);
}

void loop()
{
    // Make sure to call the indicator.loop() here. It will do all the work.
    indicator.loop();
}