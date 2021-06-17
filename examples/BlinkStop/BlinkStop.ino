/*
Example sketch to demonstrate typical use of the indicator library. This
sketch uses the builtin LED for the demonstration. 

Upload the sketch with the Arduino IDE or your usual IDE to a suitable 
board. After upload the LED should be blinking on and of at a slow rate
of 500 ms. 

Open the serial monitor if not already done, press a key and press the
send button. Now the LED should stop blinking.

To start over again, press the reset button on the board, if any, or
power the board off and on.
   
*/
#include "indicator.h"

/**
 * Define the Indicator object. It uses the predefined constant BUILTIN_LED,
 * that most boards support. If not supported by your board, change this to
 * the pin the LED is connected to (like D1, D2 etc.).
 */
Indicator indicator(BUILTIN_LED, HIGH);

/**
 * Define mode constants. Define a different byte value for each mode, and
 * give them meaningfull names. 
 */
#define MODE_BLINK_OFF 0x00
#define MODE_BLINK_SLOW 0x01

/**
 * Define the interval in milliseconds for each step. E.g. 500 means set
 * LED 500 ms on, followed by 500 ms off.
 * For the stop mode, no interval is needed.
 */
#define INTERVAL_SLOW 500

void setup()
{
    // Initialize the serial port.
    Serial.begin(9600);
    Serial.print("Press any key to stop blink");

    // Begin indicator operation. This also configures the port for the LED
    // as OUTPUT.
    indicator.begin();

    // Define the modes. Specify the define id constants and interval. You
    // may specify different values for time on and time off, but here
    // they are the same for simplicity.
    // The BLINK_OFF mode takes the id and either true (always on) or
    // false (always off). Here we use 'always off'.
    indicator.addMode(MODE_BLINK_OFF, false);
    indicator.addMode(MODE_BLINK_SLOW, INTERVAL_SLOW, INTERVAL_SLOW);

    // Select initial mode.
    indicator.setMode(MODE_BLINK_SLOW);
}

void loop()
{
    // Make sure to call the indicator.loop() here. It will do all the work.
    indicator.loop();

    // Check for keypress.
    if (Serial.available())
    {
        int input = Serial.read();
        // Select mode after keypress.
        indicator.setMode(MODE_BLINK_OFF);
    }
}