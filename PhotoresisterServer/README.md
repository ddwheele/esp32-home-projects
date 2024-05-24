This project is meant to be run with an ESP32 in this circuit.

```mermaid
---
title: Circuit diagram
---

graph LR
	subgraph ESP32
	VIN
	GND
	D35
	D34
	end
	PWR-->VIN
	GND-->Ground
	P1((Photoresistor1))
	P2((Photoresistor2))
	D35-->P1-->Ground
	D35-->[[R10K]]-->Ground
	D34-->P2-->Ground
	D34-->[[R1K]]-->Ground
```
