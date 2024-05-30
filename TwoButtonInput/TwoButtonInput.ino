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

// Button has state "pressed"
// -> if pressed long enough, button has been "pushed"
// ---> after button is pushed, there is a wait time before it can be pushed again
// -> if button pushed odd # of times, that channel is "selected"
// -> if button pushed even # of times, that channel is deselected

// Press must be this long to trigger selection change (ms)
const int PUSH_THRESHOLD = 80;

// After a selection change, stop listening for this long (ms)
unsigned long WAITING_THRESHOLD = 500UL;

// if button 1 or 2 is pressed (instantaneous)
int buttonState1 = false;
int buttonState2 = false;

// time of button press, used to count if "pushed"
unsigned long pressedTime1 = 0UL;
unsigned long pressedTime2 = 0UL;

int pushed1 = false;
int pushed2 = false;

// time of selection change, used to count "waiting time"
unsigned long changedTime1 = 0UL;
unsigned long changedTime2 = 0UL;

// is this button counting "waiting time"
int waiting1 = false;
int waiting2 = false;

// if channel 1 or 2 has been "selected" (by pushing the button for it)
int selected1 = false;
int selected2 = false;

// time a channel was selected
unsigned long selectedTime1 = 0UL;
unsigned long selectedTime2 = 0UL;

// time a channel was deselected
unsigned long deselectedTime1 = 0UL;
unsigned long deselectedTime2 = 0UL;

// time between sending data to the server (ms)
unsigned int REPORTING_INTERVAL = 10000;

// time the most recent report was sent to the server
unsigned long lastReportTime = 0UL;

// time in the last reporting interval that a channel was selected (ms)
int msSelected1 = 0;
int msSelected2 = 0;

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

int isButton1Pushed() {
  if(!isButton1Waiting()) {
    // track time the button is pressed
    if(buttonState1 == HIGH) {
      if(pushed1 == false) {
        pressedTime1 = millis();
        pushed1 = true;
      }
    } else {
      pushed1 = false;
    }
  }
  return pushed1;
}

int isButton2Pushed() {
   if(!isButton2Waiting()){
    if(buttonState2 == HIGH) {
      if(pushed2 == false) {
        pressedTime2 = millis();
        pushed2 = true;
      }
    } else {
      pushed2 = false;
    }
  }
  return pushed2;
}

// Ensures that selected1 is true if channel 2 is selected, false otherwise
void updateSelection1() {
  // if button pressed long enough, toggle selected state
  if(isButton1Pushed()) {
    currentMillis = millis();
    if((currentMillis - pressedTime1) > PUSH_THRESHOLD) {
      if(selected1 == false) {
        selected1 = true;
        selectedTime1 = millis();
      } else if(selected1 == true) {
        selected1 = false;
        deselectedTime1 = millis();
      }
      waiting1 = true;
      changedTime1 = millis();
      pushed1 = false;
    }
  }
}

// Ensures that selected2 is true if channel 2 is selected, false otherwise
void updateSelection2() {
   if(isButton2Pushed()) {
    currentMillis = millis();
    if((currentMillis - pressedTime2) > PUSH_THRESHOLD) {
      if(selected2 == false) {
        selected2 = true;
        selectedTime2 = millis();
      } else if(selected2 == true) {
        selected2 = false;
        deselectedTime2 = millis();
      }
      waiting2 = true;
      changedTime2 = millis();
      pushed2 = false;
    }
  }
}

void sendMsSelected1() {
  if(selected1) {
      if(selectedTime1 < lastReportTime) {
        // case 1: selected and has been selected
        msSelected1 = currentMillis - lastReportTime;
      } else {
        // case 2: selected and wasn't before
        msSelected1 = currentMillis - selectedTime1;
      }
    } else {
      if(deselectedTime1 < lastReportTime) {
        // case 4: not selected and wasn't before 
        msSelected1 = 0;
      } else {
        // case 3: not selected but was before
        // it turned off in this interval
        msSelected1 = deselectedTime1 - lastReportTime;
      }
    }
    noise_feed->save(msSelected1); 
    Serial.print("Sent to Noise: "); Serial.println(msSelected1);
}

void sendMsSelected2() {
  if(selected2) {
      if(selectedTime2 < lastReportTime) {
        // case 1: selected and has been selected
        msSelected2 = currentMillis - lastReportTime;
      } else {
        // case 2: selected and wasn't before
        msSelected2 = currentMillis - selectedTime2;
      }
    } else {
      if(deselectedTime2 < lastReportTime) {
        // case 4: not selected and wasn't before 
        msSelected2 = 0;
      } else {
        // case 3: not selected but was before
        // it turned off in this interval
        msSelected2 = deselectedTime2 - lastReportTime;
      }
    }
    home_feed->save(msSelected2); 
    Serial.print("Sent to Home: "); Serial.println(msSelected2);
}

void loop() {
  buttonState1 = digitalRead(BUTTON_1_PIN);
  buttonState2 = digitalRead(BUTTON_2_PIN);

  updateSelection1();
  updateSelection2();

  lightLedsIfSelected();

  //Serial.print("1: "); Serial.print(selected1); Serial.print(", 2: "); Serial.println(selected2);

  currentMillis = millis();

  if((currentMillis - lastReportTime) > REPORTING_INTERVAL) {
    Serial.print("current: "); Serial.print(currentMillis); Serial.print(",last: "); Serial.println(lastReportTime);
    sendMsSelected1();
    sendMsSelected2();
      
    lastReportTime = currentMillis;
  }

}
