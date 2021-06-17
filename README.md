# Arduino-indicator library
This library is intended to facilitate one or more LEDs, used as indicator in small devices
based on ESP8266, ESP32 or Arduino. It is capable of configuring different blinking patterns,
and selecting them in different parts of the program.

You can use it e.g. to indicate different operating modes, like setup, normal operation and 
error conditions.

# Examples
Some examples are included, to show basic applications of this library:

SimpleBlink 
   Demonstrates the use of mode selection to indicate different operating modes.
BlinkStop 
   Demonstrates that you can have modes with static on and static off states.
BlinkMorse
   Demonstrates the use of patterns to send morse code.

# Other uses
Since this library facilitates setting output pins HIGH or LOW at different patterns, it is
not limited to use for LEDs only. It is also suited e.g. to produce buzzer patterns.

# Disclaimer
 ***************************************************************************
 * Copyright (c) 2010-2021 Wim Vissers.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0
 *
 * SPDX-License-Identifier: EPL-2.0
 ***************************************************************************
 
