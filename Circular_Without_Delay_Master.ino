#include <Wire.h>
#include <SPI.h>

const int masterLedPins[] = {22, 24, 38, 36, 34, 30, 32, 28, 26, 40, 46, 44, 42};
const int masterButtonPins[] = {23, 25, 39, 37, 35, 31, 33, 29, 27, 41, 47, 45, 43};

int sequence[] = {
  0, 1, 2, 3, 4, 5, 6, 7, 8, 9,              // Master first 10
  100, 101, 102, 103,                        // Slave first 4
  10, 11, 12,                                // Master last 3
  104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116 // Remaining 16 slave
};

int currentStep = 0;
int count = 0;

const int loadPin = 53;
uint8_t digits[] = {
  B11000000, B11111001, B10100100, B10110000, B10011001,
  B10010010, B10000010, B11111000, B10000000, B10010000
};

void setup() {
  Wire.begin();
  Serial.begin(9600);

  for (int i = 0; i < 13; i++) {
    pinMode(masterLedPins[i], OUTPUT);
    pinMode(masterButtonPins[i], INPUT_PULLUP);
  }

  pinMode(loadPin, OUTPUT);
  digitalWrite(loadPin, HIGH);
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV128);

  activateCurrentStep();
}

void loop() {
  int index = sequence[currentStep];
  bool buttonPressed = false;

  while (!buttonPressed) {
    if (index < 100) {
      if (digitalRead(masterButtonPins[index]) == LOW) {
        delay(50);
        if (digitalRead(masterButtonPins[index]) == LOW) {
          digitalWrite(masterLedPins[index], LOW);
          buttonPressed = true;
        }
      }
    } else {
      Wire.requestFrom(8, 1);
      if (Wire.available()) {
        byte response = Wire.read();
        if (response == 1) {
          buttonPressed = true;
        }
      }
    }
    delay(10);
  }

  if (buttonPressed) {
    count++;
    Serial.print("Count: ");
    Serial.println(count);
    updateDisplay(count);
    currentStep++;
    if (currentStep >= sizeof(sequence) / sizeof(sequence[0])) {
      currentStep = 0;
    }
    activateCurrentStep();
  }
}

void activateCurrentStep() {
  if (currentStep >= sizeof(sequence) / sizeof(sequence[0])) return;
  int index = sequence[currentStep];

  if (index < 100) {
    digitalWrite(masterLedPins[index], HIGH);
    Serial.print("Master LED ");
    Serial.print(index);
    Serial.println(" ON");
  } else {
    Wire.beginTransmission(8);
    Wire.write(index - 100);
    Wire.endTransmission();
    Serial.print("Slave LED ");
    Serial.print(index - 100);
    Serial.println(" ON");
  }
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
}
