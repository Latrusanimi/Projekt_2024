#include <Arduino.h>

// LCD Display
#include <LiquidCrystal_I2C.h>
// I2C Komunikation
#include <Wire.h>
// Bosch Sensor
#include <Adafruit_BME680.h>
// Kommunikation mit BME 680. Wird nur verwendet, wenn I2C nicht geht
#include <SPI.h>
// ??? Verwendung noch unklar
#include <Adafruit_Sensor.h>
// Komunikation über Modem oder Bluetooth
#include <SoftwareSerial.h>
// Kameramodul
#include <Adafruit_OV7670.h>


// Definition welches Display verwendet wird.
LiquidCrystal_I2C lcd(0x27,20,4);


void setup() {
// write your initialization code here

// LCD initialisieren
    lcd.init();
    lcd.backlight();

// PIN Mode definieren
   /* pinMode(2,OUTPUT); // rote LED
    pinMode(3,OUTPUT); // gelbe LED
    pinMode(4,OUTPUT); // grüne LED */

// Geschwindigkeit der seriellen Verbindung definieren
    Serial.begin(115200);
}

void loop() {
// write your code here
}