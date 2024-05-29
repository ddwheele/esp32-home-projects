/**
 * The circuit board will have two buttons and two LEDs 
 * wired to the ESP. One LED-button pair is for channel 1 
 * and the other LED-button pair is for channel 2.
 * 
 * The ESP sends a low or a high signal for each channel 
 * once per second. Both signals start off low, and the LEDs 
 * are off. When the button is pressed for a channel that is
 * low, the LED turns on, and a high signal is sent for that
 * channel until the button is pressed again. Then the LED 
 * turns off and the low signal is sent again.
 */

#include "passwords.hpp"

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

const int BUTTON_1_PIN = 25;
const int LED_1_PIN    = 26;

const int BUTTON_2_PIN = 32;
const int LED_2_PIN    = 33;

// Push must be this long to trigger selection change
const int PUSH_THRESHOLD = 80;

// After a selection change, stop listening for this long 
unsigned long DEAD_THRESHOLD = 500UL;

// if button 1 or 2 is pressed
int buttonState1 = 0;
int buttonState2 = 0;

// how many cycles button 1 or 2 has been down
int count1 = 0;
int count2 = 0;

// time selection was changed, used to count "dead time"
unsigned long changedTime1 = 0UL;
unsigned long changedTime2 = 0UL;

// is this button counting "dead time"
int dead1 = 0;
int dead2 = 0;

// if channel 1 or 2 has been "selected" (by pushing the button for it)
int selected1 = 0;
int selected2 = 0;

unsigned long currentMillis;

AdafruitIO_Feed *noise_feed = io.feed("noise");
AdafruitIO_Feed *home_feed = io.feed("home");

void setup() {
  pinMode(BUTTON_1_PIN, INPUT);
  pinMode(BUTTON_2_PIN, INPUT);

  pinMode(LED_1_PIN, OUTPUT);
  pinMode(LED_2_PIN, OUTPUT);

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

void lightLedsIfSelected() {
  if(selected1) {
     digitalWrite(LED_1_PIN, HIGH);
  } else {
     digitalWrite(LED_1_PIN, LOW);
  }
  if(selected2) {
     digitalWrite(LED_2_PIN, HIGH);
  } else {
     digitalWrite(LED_2_PIN, LOW);
  }
}

int isButton1Dead() {
  if(dead1) {
    currentMillis = millis();
    if((currentMillis - changedTime1) > DEAD_THRESHOLD) {
      dead1=false;
    }
  }
  return dead1;
}

int isButton2Dead() {
  if(dead2) {
    currentMillis = millis();
    if((currentMillis - changedTime2) > DEAD_THRESHOLD) {
      dead2=false;
    }
  }
   return dead2;
}


void loop() {
  buttonState1 = digitalRead(BUTTON_1_PIN);
  buttonState2 = digitalRead(BUTTON_2_PIN);

// don't count if button was just pressed
if(!isButton1Dead()) {
  // track time the button is pressed
  if(buttonState1 == HIGH) {
    count1++;
  } else {
    count1 = 0;
  }
}

if(!isButton2Dead()){
  if(buttonState2 == HIGH) {
    count2++;
  } else {
    count2 = 0;
  }
}

// if button pressed long enough, toggle selected state
  if(count1 > PUSH_THRESHOLD) {
    if(selected1 == false) {
      selected1 = true;
    } else if(selected1 == true) {
      selected1 = false;
    }
    dead1 = true;
    changedTime1 = millis();
    count1 = 0;
  }
  if(count2 > PUSH_THRESHOLD) {
    if(selected2 == false) {
      selected2 = true;
    } else if(selected2 == true) {
      selected2 = false;
    }
    dead2 = true;
    changedTime2 = millis();
    count2 = 0;
  }

  lightLedsIfSelected();

}
