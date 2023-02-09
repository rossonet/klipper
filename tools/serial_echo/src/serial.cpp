#include <Arduino.h>

void establishContact() {
  while (Serial.available() <= 0 && Serial1.available() <= 0 && Serial2.available() <= 0) {
    Serial.print("A\n");   // send a capital A
    Serial1.print("B\n");   // send a capital A
    Serial2.print("C\n");   // send a capital A
    delay(1000);
  }
}

void setup() {
  Serial.begin(250000);
  Serial1.begin(250000);
  Serial2.begin(250000);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  establishContact();  // send a byte to establish contact until receiver responds
}

void loop() {
  // if we get a valid byte, read analog ins:
  if (Serial.available() > 0 || Serial1.available() > 0 || Serial2.available() > 0) {
    delay(600);
    String serialChar = "- ";
    if (Serial.available() > 0) {
      serialChar += Serial.readString();
    }
    if (Serial1.available() > 0) {
      serialChar += Serial1.readString();
    }
    if (Serial2.available() > 0) {
      serialChar += Serial2.readString();
    }
    Serial.print("data ");
    Serial.print(serialChar);
    Serial.print("\n");
    Serial1.print("data1 ");
    Serial1.print(serialChar);
    Serial1.print("\n");
    Serial2.print("data2 ");
    Serial2.print(serialChar);
    Serial2.print("\n");
  }
}

