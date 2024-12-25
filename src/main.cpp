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

// Wird benötigt wegen der Funktion malloc in der Adafruit OV7670 Bibliothek
#include <stdlib.h>



//I2C Adresse Sensor vergeben
#define BME680_I2C_ADDRESS 0x76

Adafruit_BME680 bme680;

// Definition welches Display verwendet wird.
LiquidCrystal_I2C lcd(0x27,20,4);

// Anschluss SIM7600g-h Modul definieren
SoftwareSerial sim7600g(1,0);

//ThingSpeak API Keys
const char* api_key_bme680 = "VFNZDUII0ENDF526";

//URL Thingspeak
const char* url_bme680 = "https://api.thingspeak.com/update";


//Definition Funktion Daten senden
void sendBME680Data(float temperature, float humidity, float pressure);

void setup() {
// write your initialization code here

// LCD initialisieren
    lcd.init();
    lcd.backlight();

// PIN Mode definieren
   /* pinMode(2,OUTPUT); // rote LED
    pinMode(3,OUTPUT); // gelbe LED
    pinMode(4,OUTPUT); // grüne LED */

// BME 680 initialisieren
    // BME680_OS_8X bedeutet, dass 8 Messungen durchgeführt werden und der Mittelwert genommen wird.
    // es sind 0, 1, 2, 4, 8 und 16 Messungen pro Abfrage möglich.
    bme680.setHumidityOversampling(BME680_OS_8X);
    bme680.setPressureOversampling(BME680_OS_8X);
    bme680.setTemperatureOversampling(BME680_OS_8X);
    // wird benötigt um Rauschen / Schwankungen im Messergebnis zu glätten
    // gemäss ChatGPT
    bme680.setIIRFilterSize(BME680_FILTER_SIZE_3);

// Geschwindigkeit der seriellen Verbindung definieren
    Serial.begin(115200);
}

void loop() {
// write your code here
    int verbindung = 0;
    float temperatur = bme680.readTemperature();
    float feuchtigkeit = bme680.readHumidity();
    float luftdruck = bme680.readPressure()/100000;

// Ausgabe auf Display
    lcd.clear();
    // Zeile 1
    lcd.setCursor(0, 0);
    lcd.print("Temperatur: ");
    lcd.print(temperatur);
    lcd.print("°C");
    // Zeile 2
    lcd.setCursor(0, 1);
    lcd.print("Feuchtigkeit: ");
    lcd.print(feuchtigkeit);
    lcd.print("%");
    // Zeile 3
    lcd.setCursor(0, 2);
    lcd.print("Luftdruck: ");
    lcd.print(luftdruck);
    lcd.print(" bar");
    // Zeile 4
    lcd.setCursor(0, 3);
    if (verbindung == 1) {
        lcd.print("Verbindung okay");
    }
    else if (verbindung == 0) {
        lcd.print("Verbindung nicht okay");
    }
    else {
        lcd.print("Unbekannter Fehler");
    }


    sendBME680Data(temperatur, feuchtigkeit, luftdruck);


    delay(1000);
   // millis(1000);


}

void sendBME680Data(float temperature, float humidity, float pressure) {
    String url = String(url_bme680) + "?api_key=" + api_key_bme680 +
                 "&field1=" + String(temperature) +
                 "&field2=" + String(humidity) +
                 "&field3=" + String(pressure);

    sim7600g.println("AT+HTTPINIT");
    delay(100);
    sim7600g.println("AT+HTTPPARA=\"URL\",\"" + url + "\"");
    delay(100);
    sim7600g.println("AT+HTTPACTION=0");
    delay(3000);
    sim7600g.println("AT+HTTPTERM");
}