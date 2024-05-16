#include <Arduino.h>
#include <U8g2lib.h>

void setup() {
    Serial.begin(9600);
    Serial.println("Hello World!");

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
    Serial.println("I'm alive!");
}