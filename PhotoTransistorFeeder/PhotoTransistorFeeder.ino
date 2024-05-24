// Adafruit IO Publish & Subscribe, Digital Input and Output Example
//
// Adafruit invests time and resources in providing this open-source code.
// Please support Adafruit and open-source hardware by purchasing
// products from Adafruit!
//
// Written by Todd Treece for Adafruit Industries
// Modified by Brent Rubell for Adafruit Industries
// Copyright (c) 2020 Adafruit Industries
// Licensed under the MIT license.
//
// All text above must be included in any redistribution

/**
 * Example code demonstrated sending a value to the Adafruit IO Server.
 * I modified it to send the values from two phototransistors.
 */

// passwords.h must define:
// WIFI_SSID and WIFI_PASS - wifi name and password
// IO_USERNAME and IO_KEY - adafruit IO key
#include "passwords.hpp"


// comment out the following lines if you are using fona or ethernet
#include "AdafruitIO_WiFi.h"

#if defined(USE_AIRLIFT) || defined(ADAFRUIT_METRO_M4_AIRLIFT_LITE) ||         \
    defined(ADAFRUIT_PYPORTAL)
// Configure the pins used for the ESP32 connection
#if !defined(SPIWIFI_SS) // if the wifi definition isnt in the board variant
// Don't change the names of these #define's! they match the variant ones
#define SPIWIFI SPI
#define SPIWIFI_SS 10  // Chip select pin
#define SPIWIFI_ACK 9  // a.k.a BUSY or READY pin
#define ESP32_RESETN 6 // Reset pin
#define ESP32_GPIO0 -1 // Not connected
#endif
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS, SPIWIFI_SS,
                   SPIWIFI_ACK, ESP32_RESETN, ESP32_GPIO0, &SPIWIFI);
#else
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);
#endif

/************************ Example Starts Here *******************************/

const int pin10K = 35;
const int pin1K = 35;

int value10K = 0;
int value1K = 0;

AdafruitIO_Feed *tenk_feed = io.feed("tenk-phototransistor");
AdafruitIO_Feed *onek_feed = io.feed("onek-phototransistor");

int readValue10K() {
  value10K = analogRead(pin10K);
  Serial.println(value10K);
  return value10K;
}

int readValue1K() {
  value1K = analogRead(pin1K);
  Serial.println(value1K);
  return value1K;
}

void setup() {

  pinMode(pin10K, INPUT);
  pinMode(pin1K, INPUT);

  // start the serial connection
  Serial.begin(115200);

  // wait for serial monitor to open
  while(! Serial);

  Serial.print("Connecting to Adafruit IO");

  // connect to io.adafruit.com
  io.connect();

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());
}

void loop() {
  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();

  // grab the values
  value10K=readValue10K();
  value1K=readValue1K();
 
  // save values to the feeds on adafruit io
  Serial.print("sending tenk -> "); Serial.println(value10K);
  tenk_feed->save(value10K);
  Serial.print("sending onek -> "); Serial.println(value1K);
  onek_feed->save(value1K);
  delay(60000);
}
