#include <Wire.h>
#include <SPI.h>

const int ledPins[] = {22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46};
const int buttonPins[] = {23, 25, 27, 29, 31, 33, 35, 37, 39, 41, 43, 45, 47};

int count = 0;
int currentLED = -1;
int lastLED = -1;

uint8_t digits[] = {
    B11000000, B11111001, B10100100, B10110000, B10011001, 
    B10010010, B10000010, B11111000, B10000000, B10010000
};

const int loadPin = 53;

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
    lightRandomLED();
}

void loop() {
    if (currentLED < 13) {
        // Check master button
        if (digitalRead(buttonPins[currentLED]) == LOW) {
            delay(50);
            if (digitalRead(buttonPins[currentLED]) == LOW) {
                Serial.print("Button Pressed (Master): LED ");
                Serial.println(currentLED);
                handleButtonPress();
            }
        }
    } else {
        // Check slave button with timeout
        Wire.requestFrom(8, 1);
        unsigned long startTime = millis();
        while (!Wire.available()) {
            if (millis() - startTime > 100) {
                Serial.println("Slave Response Timeout!");
                return;
            }
        }
        if (Wire.available()) {
            int buttonPressed = Wire.read();
            if (buttonPressed == 1) {
                Serial.print("Button Pressed (Slave): LED ");
                Serial.println(currentLED - 13);
                handleButtonPress();
            }
        }
    }
}

void handleButtonPress() {
    count++;
    Serial.print("Count Updated: ");
    Serial.println(count);
    updateDisplay(count);
    delay(300);
    lightRandomLED();
}

void lightRandomLED() {
    // Turn off previous LED
    if (currentLED != -1) {
        if (currentLED < 13) {
            digitalWrite(ledPins[currentLED], LOW);
        } else {
            Wire.beginTransmission(8);
            Wire.write(255);
            Wire.endTransmission();
        }
    }

    // Select a new random LED
    int newLED;
    do {
        newLED = random(0, 33);
    } while (newLED == lastLED);
    lastLED = newLED;

    // Glow LED and print status
    if (newLED < 13) {
        currentLED = newLED;
        digitalWrite(ledPins[currentLED], HIGH);
        Serial.print("LED ");
        Serial.print(currentLED);
        Serial.println(" (Master) is ON");
    } else {
        currentLED = newLED;
        Wire.beginTransmission(8);
        Wire.write(newLED - 13);
        Wire.endTransmission();
        Serial.print("LED ");
        Serial.print(newLED - 13);
        Serial.println(" (Slave) is ON");
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
    Serial.print("Display Updated: ");
    Serial.println(num);
}
