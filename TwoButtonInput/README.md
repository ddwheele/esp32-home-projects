# Two Button Input

The circuit board will have two buttons and two LEDs wired to the ESP. One LED-button pair is for channel 1 and the other LED-button pair is for channel 2.

The ESP sends a low or a high signal for each channel once per second. Both signals start off low, and the LEDs are off. When the button is pressed for a channel that is low, the LED turns on, and a high signal is sent for that channel until the button is pressed again. Then the LED turns off and the low signal is sent again.

This program is meant to be run with the ESP wired as shown below:
```mermaid
---
title: Block Circuit Diagram
---
block-beta
block:ESP32
	VIN
	GND
	D26
	D25
	D33
	D32
end

block:LED1
	columns 2
	LED1:2
	L1plus["+"] L1minus["-"]
end

block:LED2
columns 2
	LED2:2
	L2plus["+"] L2minus["-"]
end

block:Button1
columns 2
	Button1:2
	B1plus["+"] B1minus["-"]
end

block:Button2
columns 2
	Button2:2
	B2plus["+"] B2minus["-"]
end

R1_10K[["R10K"]]
R1_220[["R220"]]
R2_10K[["R10K"]]
R2_220[["R220"]]


VIN --- B1plus
B1minus --- R1_10K 
R1_10K --- Ground
B1minus --- D25
D26 --- L1plus
L1minus --- R1_220
R1_220 --- Ground

VIN --- B2plus
B2minus --- R2_10K
R2_10K --- Ground
B2minus --- D32
D33 --- L2plus
L2minus --- R2_220
R2_220 --- Ground

```

```mermaid
---
title: Flow Circuit diagram (backup)
---

graph LR
	subgraph ESP32
	VIN
	GND
	D25
	D26
	D32
	D33
	end
	GND --- Ground
	B1((Button1))
	B2((Button2))
	R1_10K[[R10K]]
	R1_220[[R220]]
	R2_10K[[R10K]]
	R2_220[[R220]]
	L1((LED1))
	L2((LED2))
	VIN --- B1 --- R1_10K --- Ground
	B1 --- D25
	D26 --- L1 --- R1_220 --- Ground

	VIN --- B2 --- R2_10K --- Ground
	B2 --- D32
	D33 --- L2 --- R2_220 --- Ground
```
