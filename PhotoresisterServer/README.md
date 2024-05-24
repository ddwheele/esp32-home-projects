This project reads values from two phototransistors and serves a webpage with the latest values. It is bBased on the tutorial code for the LAFVIN ESP32 Basic Starter Kit, Project 9 ESP32 DHT11 Web Server.

It must be run with an ESP32 wired as shown below. PTL stands for Photo Transistor Light http://adafru.it/2831

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
	P1((PTL1))
	P2((PTL2))
	R10[[R10K]]
	R1[[R1K]]
	D35-->P1-->Ground
	D35-->R10-->Ground
	D34-->P2-->Ground
	D34-->R1-->Ground
```
