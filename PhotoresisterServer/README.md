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
	R10[[R10K]]
	R1[[R1K]]
	D35-->P1-->Ground
	D35-->R10-->Ground
	D34-->P2-->Ground
	D34-->R1-->Ground
```
