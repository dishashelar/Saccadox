#include <Wire.h>

const int ledPins[] = {2, 4, 6, 8, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52};
const int buttonPins[] = {3, 5, 7, 9, 23, 25, 27, 29, 31, 33, 35, 37, 39, 41, 43, 45, 47, 49, 51, 53};

int currentLed = -1;
bool buttonPressedFlag = false;

void setup() {
  Wire.begin(8);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  Serial.begin(9600);
  Serial.println("Slave Starting...");

  for (int i = 0; i < 20; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
}

void loop() {
  if (currentLed != -1 && digitalRead(buttonPins[currentLed]) == LOW) {
    delay(50);
    if (digitalRead(buttonPins[currentLed]) == LOW) {
      buttonPressedFlag = true;
      digitalWrite(ledPins[currentLed], LOW);
      currentLed = -1;
    }
  }
}

void receiveEvent(int bytes) {
  if (Wire.available()) {
    int command = Wire.read();
    if (command == 255) {
      for (int i = 0; i < 20; i++) {
        digitalWrite(ledPins[i], LOW);
      }
      currentLed = -1;
    } else if (command >= 0 && command < 20) {
      for (int i = 0; i < 20; i++) {
        digitalWrite(ledPins[i], LOW);
      }
      digitalWrite(ledPins[command], HIGH);
      currentLed = command;
      buttonPressedFlag = false;
    }
  }
}

void requestEvent() {
  Wire.write(buttonPressedFlag ? 1 : 0);
  buttonPressedFlag = false;
}
