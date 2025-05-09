#include <Wire.h>

const int slaveLedPins[] = {28, 26, 8, 22, 50, 48, 36, 32, 30, 24, 6, 4, 2, 34, 38, 40, 42, 44, 46, 52};
const int slaveButtonPins[] = {29, 27, 9, 23, 51, 49, 37, 33, 31, 25, 7, 5, 3, 35, 39, 41, 43, 45, 47, 53};

int currentLed = -1;

void setup() {
  Wire.begin(8);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  Serial.begin(9600);
  for (int i = 0; i < 20; i++) {
    pinMode(slaveLedPins[i], OUTPUT);
    pinMode(slaveButtonPins[i], INPUT_PULLUP);
  }
}

void loop() {}

void receiveEvent(int howMany) {
  if (Wire.available()) {
    currentLed = Wire.read();

    for (int i = 0; i < 20; i++) {
      digitalWrite(slaveLedPins[i], LOW);
    }

    if (currentLed >= 0 && currentLed < 20) {
      digitalWrite(slaveLedPins[currentLed], HIGH);
      Serial.print("Slave LED ");
      Serial.print(currentLed);
      Serial.println(" ON");
    }
  }
}

void requestEvent() {
  if (currentLed >= 0 && currentLed < 20) {
    bool pressed = (digitalRead(slaveButtonPins[currentLed]) == LOW);
    if (pressed) {
      delay(50);
      pressed = (digitalRead(slaveButtonPins[currentLed]) == LOW);
    }

    if (pressed) {
      Serial.print("Slave Button ");
      Serial.print(currentLed);
      Serial.println(" Pressed");
      digitalWrite(slaveLedPins[currentLed], LOW);
      Wire.write(1);
      currentLed = -1;
      return;
    }
  }
  Wire.write(0);
}
