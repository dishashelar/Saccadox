#include <Wire.h>
#include <SPI.h>

const int ledPins[] = {22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46};
const int buttonPins[] = {23, 25, 27, 29, 31, 33, 35, 37, 39, 41, 43, 45, 47};

int count = 0;
const int loadPin = 53;
int currentLED = -1;

uint8_t digits[] = {
  B11000000, B11111001, B10100100, B10110000, B10011001,
  B10010010, B10000010, B11111000, B10000000, B10010000
};

bool usedLEDs[33] = {false};
int totalUsed = 0;

void setup() {
  Wire.begin();
  Serial.begin(9600);
  Serial.println("Master Starting...");

  for (int i = 0; i < 13; i++) {
    pinMode(ledPins[i], OUTPUT);
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  pinMode(loadPin, OUTPUT);
  digitalWrite(loadPin, HIGH);
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV128);
  randomSeed(analogRead(0));
}

void loop() {
  lightRandomLED();

  unsigned long startTime = millis();
  bool pressed = false;

  while (millis() - startTime < 1500) {
    if (currentLED < 13) {
      if (digitalRead(buttonPins[currentLED]) == LOW) {
        delay(50);
        if (digitalRead(buttonPins[currentLED]) == LOW) {
          pressed = true;
          break;
        }
      }
    } else {
      Wire.requestFrom(8, 1);
      if (Wire.available()) {
        if (Wire.read() == 1) {
          pressed = true;
          break;
        }
      }
    }
  }

  if (pressed) {
    count++;
    updateDisplay(count);
    Serial.print("Button Pressed on LED ");
    Serial.println(currentLED);
  } else {
    Serial.print("Timeout on LED ");
    Serial.println(currentLED);
  }

  turnOffCurrentLED();
  delay(100);
}

void lightRandomLED() {
  if (totalUsed >= 33) {
    memset(usedLEDs, 0, sizeof(usedLEDs));
    totalUsed = 0;
  }

  int newLED;
  do {
    newLED = random(0, 33);
  } while (usedLEDs[newLED]);

  currentLED = newLED;
  usedLEDs[newLED] = true;
  totalUsed++;

  if (currentLED < 13) {
    digitalWrite(ledPins[currentLED], HIGH);
    Serial.print("LED ");
    Serial.print(currentLED);
    Serial.println(" (Master) ON");
  } else {
    Wire.beginTransmission(8);
    Wire.write(currentLED - 13);
    Wire.endTransmission();
    Serial.print("LED ");
    Serial.print(currentLED - 13);
    Serial.println(" (Slave) ON");
  }
}

void turnOffCurrentLED() {
  if (currentLED < 13) {
    digitalWrite(ledPins[currentLED], LOW);
  } else {
    Wire.beginTransmission(8);
    Wire.write(255);
    Wire.endTransmission();
  }
  currentLED = -1;
}

void updateDisplay(int num) {
  int tens = num / 10;
  int ones = num % 10;
  uint8_t dataToSend[] = {digits[ones], digits[tens]};

  SPI.setDataMode(SPI_MODE0);
  delay(2);
  digitalWrite(loadPin, LOW);
  SPI.transfer(dataToSend[0]);
  SPI.transfer(dataToSend[1]);
  digitalWrite(loadPin, HIGH);
  Serial.print("Display: ");
  Serial.println(num);
}
