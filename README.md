# Two Button Input

The circuit board will have two buttons and two LEDs wired to the ESP. One LED-button pair is for channel 1 and the other LED-button pair is for channel 2.

The ESP sends a low or a high signal for each channel once per second. Both signals start off low, and the LEDs are off. When the button is pressed for a channel that is low, the LED turns on, and a high signal is sent for that channel until the button is pressed again. Then the LED turns off and the low signal is sent again.

This program is meant to be run with the ESP wired as shown below:

```mermaid
---
title: Circuit diagram
---

graph LR
	subgraph ESP32
	3V3
	GND
	D4
	D5
	TBD1
	TBD2
	end
	GND--Ground
	B1((Button1))
	B2((Button2))
	R1_10K[[R10K]]
	R1_220[[R220]]
	R2_10K[[R10K]]
	R2_220[[R220]]
	L1((LED1))
	L2((LED2))
	3V3--B1
	B1--R1_10K--Ground
	B1--D4
	D5--L1--R1_220--Ground
	3V3--B2--R2_10K--Ground
	B2--TBD1
	TBD2--L2--R2_220--Ground
```
