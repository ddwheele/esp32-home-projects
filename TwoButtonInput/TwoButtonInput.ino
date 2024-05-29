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
unsigned long WAITING_THRESHOLD = 500UL;

// if button 1 or 2 is pressed
int buttonState1 = false;
int buttonState2 = false;

// time of button press, used to count if "pushed"
unsigned long pressedTime1 = 0UL;
unsigned long pressedTime2 = 0UL;

int pressed1 = false;
int pressed2 = false;

// time of selection change, used to count "waiting time"
unsigned long changedTime1 = 0UL;
unsigned long changedTime2 = 0UL;

// is this button counting "waiting time"
int waiting1 = false;
int waiting2 = false;

// if channel 1 or 2 has been "selected" (by pushing the button for it)
int selected1 = false;
int selected2 = false;

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

int isButton1Waiting() {
  if(waiting1) {
    currentMillis = millis();
    if((currentMillis - changedTime1) > WAITING_THRESHOLD) {
      waiting1=false;
    }
  }
  return waiting1;
}

int isButton2Waiting() {
  if(waiting2) {
    currentMillis = millis();
    if((currentMillis - changedTime2) > WAITING_THRESHOLD) {
      waiting2=false;
    }
  }
   return waiting2;
}

int isButton1Pressed() {
  if(!isButton1Waiting()) {
    // track time the button is pressed
    if(buttonState1 == HIGH) {
      if(pressed1 == false) {
        pressedTime1 = millis();
        pressed1 = true;
      }
    } else {
      pressed1 = false;
    }
  }
  return pressed1;
}

int isButton2Pressed() {
   if(!isButton2Waiting()){
    if(buttonState2 == HIGH) {
      if(pressed2 == false) {
        pressedTime2 = millis();
        pressed2 = true;
      }
    } else {
      pressed2 = false;
    }
  }
  return pressed2;
}

void updateSelection1() {
  // if button pressed long enough, toggle selected state
  if(isButton1Pressed()) {
    currentMillis = millis();
    if((currentMillis - pressedTime1) > PUSH_THRESHOLD) {
      if(selected1 == false) {
        selected1 = true;
      } else if(selected1 == true) {
        selected1 = false;
      }
      waiting1 = true;
      changedTime1 = millis();
      pressed1 = false;
    }
  }
}

void updateSelection2() {
   if(isButton2Pressed()) {
    currentMillis = millis();
    if((currentMillis - pressedTime2) > PUSH_THRESHOLD) {
      if(selected2 == false) {
        selected2 = true;
      } else if(selected2 == true) {
        selected2 = false;
      }
      waiting2 = true;
      changedTime2 = millis();
      pressed2 = false;
    }
  }
}

void loop() {
  buttonState1 = digitalRead(BUTTON_1_PIN);
  buttonState2 = digitalRead(BUTTON_2_PIN);

  updateSelection1();

  updateSelection2();
 
  lightLedsIfSelected();
}
