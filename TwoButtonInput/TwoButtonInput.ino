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

const int button1 = 25;
const int led1    = 26;

const int button2 = 32;
const int led2    = 33;

int buttonState1 = 0;
int buttonState2 = 0;

void setup() {
  Serial.begin(115200);

  pinMode(button1, INPUT);
  pinMode(button2, INPUT);

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
}

void loop() {
  buttonState1 = digitalRead(button1);
  buttonState2 = digitalRead(button2);
  
  Serial.print("1: ");   Serial.println(buttonState1);
 // Serial.print("2: ");  Serial.println(buttonState2);

// turn the corresponding LED on when the button is pressed
  if(buttonState1 == HIGH) {
    digitalWrite(led1, HIGH);
  } else {
    digitalWrite(led1, LOW);
  }

  if(buttonState2 == HIGH) {
    digitalWrite(led2, HIGH);
  } else {
    digitalWrite(led2, LOW);
  }
}
