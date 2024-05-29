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

const  int BUTTON_1_PIN = 25;
const  int LED_1_PIN    = 26;

const  int BUTTON_2_PIN = 32;
const  int LED_2_PIN    = 33;

const  int PUSH_THRESHOLD = 200;

// if button 1 or 2 is pressed
int buttonState1 = 0;
int buttonState2 = 0;

// how many cycles button 1 or 2 has been down
int count1 = 0;
int count2 = 0;

// if channel 1 or 2 has been "selected" (by pushing the button for it)
int selected1 = 0;
int selected2 = 0;


void setup() {
  Serial.begin(115200);

  pinMode(BUTTON_1_PIN, INPUT);
  pinMode(BUTTON_2_PIN, INPUT);

  pinMode(LED_1_PIN, OUTPUT);
  pinMode(LED_2_PIN, OUTPUT);
}

void loop() {
  buttonState1 = digitalRead(BUTTON_1_PIN);
  buttonState2 = digitalRead(BUTTON_2_PIN);

// track time the button is pressed
  if(buttonState1 == HIGH) {
    count1++;
  } else {
    count1 = 0;
  }

// if button pressed long enough, toggle selected state
  if(count1 > PUSH_THRESHOLD) {
    if(selected1 == 0) {
      selected1 = 1;
    } else if(selected1 == 1) {
      selected1 = 0;
    }
    count1 = 0;
    Serial.print("====================================");
  }

// if selected, light the LED
  if(selected1) {
     digitalWrite(LED_1_PIN, HIGH);
  } else {
     digitalWrite(LED_1_PIN, LOW);
  }

if(count1 > 0) {
   Serial.print("1: "); Serial.print(count1);Serial.print(", ");Serial.println(selected1);
}

  if(buttonState2 == HIGH) {
    digitalWrite(LED_2_PIN, HIGH);
    count2++;
  } else {
    digitalWrite(LED_2_PIN, LOW);
    count2 = 0;
  }
}
