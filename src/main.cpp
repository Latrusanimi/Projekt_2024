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
// Komunikation mittels Modem oder Bluetooth
#include <SoftwareSerial.h>
// Kameramodul

// Wird benötigt wegen der Funktion malloc in der Adafruit OV7670 Bibliothek
// include <stdatomic.h> // Fehler in der Bibliothek?
#include <stdlib.h>



// I2C Adresse Sensor vergeben
#define BME680_I2C_ADDRESS 0x77

Adafruit_BME680 bme680;

// Definition welches Display verwendet wird.
LiquidCrystal_I2C lcd(0x27,20,4);

// WLAN Daten
const char* ssid = "Projekt";
const char* password = "C505v33p";

// ThingSpeak API Keys
const char* api_key_bme680 = "VFNZDUII0ENDF526";  // Sicherheitsrisiko, da API Key unverschlüssselt. in einer späteren Version anzupassen
// URL Thingspeak
const char* url_bme680 = "http://api.thingspeak.com/update";

// Definition Funktion WLAN Verbindungsaufbau
void verbindungWlan();

// Definition Funktion AT Befehle senden und Antwort auswerten
bool sendAT(const char* befehl, const char* antwortErwartet, unsigned long timeout);

// Definition Funktion Daten senden
void sendBME680Data(float temperature, float humidity, float pressure);

// Definition Funktion 2004 Display
void anzeigeDisplay(float temperatur, float feuchtigkeit, float luftdruck, String antwortESP01);

//bool netzwerkTest ();

void setup() {
    // write your initialization code here

    //sim7600g.begin(19200);
    Wire.begin();


    // LCD initialisieren
    lcd.init();
    lcd.backlight();

    // PIN Mode definieren
    // Pins werden später benötigt
    /*
    pinMode(2,OUTPUT); // rote LED
    pinMode(3,OUTPUT); // gelbe LED
    pinMode(4,OUTPUT); // grüne LED*/

    // BME 680 initialisieren
    // BME680_OS_8X bedeutet, dass 8 Messungen durchgeführt werden und der Mittelwert genommen wird.
    // es sind 0, 1, 2, 4, 8 und 16 Messungen pro Abfrage möglich.
    bme680.setHumidityOversampling(BME680_OS_8X);
    bme680.setPressureOversampling(BME680_OS_8X);
    bme680.setTemperatureOversampling(BME680_OS_8X);
    // wird benötigt um Rauschen / Schwankungen im Messergebnis zu glätten
    // gemäss ChatGPT
    bme680.setIIRFilterSize(BME680_FILTER_SIZE_3);
    bme680.begin(BME680_I2C_ADDRESS);

    // Geschwindigkeit der seriellen Verbindung definieren
    Serial.begin(115200);

    // Verbindungsaufbau
    lcd.setCursor(0,0);
    lcd.print("Initialisierung");
    delay(1000);
    verbindungWlan();

    // Timer damit WLAN Zeit hat für Verbindungsaufbau
    for (int i = 60; i >= 0; i--) {
        delay(1000); // 1 Sekunde warten
        lcd.setCursor(0, 1);
        lcd.print("                "); // Zeile leeren
        lcd.setCursor(0, 1);
        lcd.print(i);
    }
    delay(1000);
    lcd.clear();
}


void loop() {
    // write your code here
    bme680.performReading();

    //bool statusVerbindung = netzwerkTest();
    float temperatur = bme680.readTemperature();
    float feuchtigkeit = bme680.readHumidity();
    float luftdruck = bme680.readPressure()/100000;
    String antwortESP01 = "";

    // Ausgabe auf Display
    anzeigeDisplay(temperatur, feuchtigkeit, luftdruck, antwortESP01);

    delay(10000);


    // Daten an ThingSpeak senden
    sendBME680Data(temperatur, feuchtigkeit, luftdruck);


    delay(10000);

}

void verbindungWlan() {
    /*sendAT("AT","OK",1000);
    sendAT("AT+CWMODE=1","OK",1000);
    String befehl = String("AT+CWJAP=\"") + String(ssid) + String("\",\"") + String(password) + String("\"");
    sendAT(befehl.c_str(),"WIFI CONNECTED",10000);
    */

    if(!sendAT("AT","OK",5000)) {
        lcd.setCursor(0,3);
        lcd.print("Keine Verbindung   ");
        return;
    }
    if(!sendAT("AT+CWMODE=1","OK",5000)) {
        lcd.setCursor(0,3);
        lcd.print("Fehler WLAN Modus  ");
        return;
    }

    String befehl = String("AT+CWJAP=\"") + ssid + "\",\"" + password + "\"";
    if (!sendAT(befehl.c_str(), "WIFI CONNECTED", 45000)) {
        lcd.setCursor(0,3);
        lcd.print("WLAN nicht verbunden");
        return;
    }

    lcd.setCursor(0,3);
    lcd.print("WLAN Verbunden     ");
    }

bool sendAT(const char* befehl, const char* antwortErwartet, unsigned long timeout) {
    Serial.println(befehl);
    unsigned long start = millis();

    String antwort = "";
    while(millis() - start < timeout) {
        if(Serial.available()) {
            char c = Serial.read();
            antwort += c;

            if(antwort.indexOf(antwortErwartet) != -1) {
                lcd.setCursor(0,2);
                lcd.print("Antwort WLAN-Modul: ");
                lcd.setCursor(0,3);
                lcd.print(antwort);
                return true;
            }
        }
    }
    lcd.setCursor(0,2);
    lcd.print("Antwort WLAN-Modul: ");
    lcd.setCursor(0,3);
    lcd.print("ERROR 404           ");
    return false;
}

// Funktion Daten an Server senden
void sendBME680Data(float temperature, float humidity, float pressure) {
    String url = String(url_bme680) + "?api_key=" + api_key_bme680 +
                 "&field1=" + String(temperature) +
                 "&field2=" + String(humidity) +
                 "&field3=" + String(pressure);

    // LCD-Ausgabe
    lcd.clear();
    lcd.setCursor(0, 3);
    lcd.print("Sende Daten...      ");

    /*// AT-Befehl für HTTP-GET
    String atCommand = String("AT+HTTPCLIENT=2,0,\"") + url + String("\",,1"); // HTTP GET Anfrage
    if (sendAT(atCommand.c_str(), "OK", 10000)) {
        lcd.setCursor(0, 3);
        lcd.print("Daten gesendet!  ");
    } else {
        lcd.setCursor(0, 3);
        lcd.print("Senden fehlgeschl.! ");
    }
    */
    // Verbindung zum Server herstellen
    if (!sendAT("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80", "CONNECT", 5000)) {
        lcd.setCursor(0, 3);
        lcd.print("No Server connection");
        return;
    }

    // Alternative AT Commands gemäss ChatGPT
    // HTTP-GET-Request vorbereiten
    String getRequest = String("GET /update?api_key=") + api_key_bme680 +
                    "&field1=" + String(temperature) +
                    "&field2=" + String(humidity) +
                    "&field3=" + String(pressure) +
                    " HTTP/1.1\r\n" +
                    "Host: api.thingspeak.com\r\n" +
                    "Connection: close\r\n\r\n";

        /*String("GET ") + url + " HTTP/1.1\r\n" +
                        "Host: api.thingspeak.com\r\n" +
                        "Connection: close\r\n\r\n";
                        */

    // Länge des Requests berechnen und senden
    String cipsendCommand = String("AT+CIPSEND=") + getRequest.length();

    if (!sendAT(cipsendCommand.c_str(), ">", 5000)) {
        lcd.setCursor(0, 3);
        lcd.print("CIPSEND ERROR       ");
        return;
    }

    // Sende den HTTP-Request
    Serial.print(getRequest);

    // Warten auf die Antwort
    if (!sendAT("", "CLOSED", 10000)) {
        lcd.setCursor(0, 3);
        lcd.print("Fehler bei HTTP!    ");
        return;
    }

    lcd.setCursor(0, 3);
    lcd.print("Daten gesendet!  :) ");

}

// Funktion Displayanzeige
void anzeigeDisplay(float temperatur, float feuchtigkeit, float luftdruck, String antwortESP01) {
    lcd.clear();
    // Zeile 1
    lcd.setCursor(0, 0);
    lcd.print("Temperatur:   ");
    lcd.print(temperatur,1);
    lcd.print(" C");
    // Zeile 2
    lcd.setCursor(0, 1);
    lcd.print("Feuchtigkeit: ");
    lcd.print(feuchtigkeit,1);
    lcd.print(" %");
    // Zeile 3
    lcd.setCursor(0, 2);
    lcd.print("Luftdruck:   ");
    lcd.print(luftdruck,1);
    lcd.print(" bar");
    // Zeile 4
    //lcd.setCursor(0, 3);
    //lcd.print(antwortESP01);
}
