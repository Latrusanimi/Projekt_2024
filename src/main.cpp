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
void anzeigeDisplay(float temperatur, float feuchtigkeit, float luftdruck);

bool softwareDebuggingMode = false;
bool debuggingMode = false;

void setup() {
    // write your initialization code here


    // Debugging Mode extern forcieren
    if (PIN5 == LOW) {
        debuggingMode = true;
    } else {
        debuggingMode = softwareDebuggingMode;
    }

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
    pinMode(5, INPUT_PULLUP); // Debugging Modus forcieren

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
    lcd.print("Initialisierung:    ");
    delay(1000);
    // Timer damit WLAN Zeit hat für Verbindungsaufbau
    unsigned long alteZeit = 0;
    for (int i = 60; i > 0;) {
        if (i > 59) {
            verbindungWlan();
        }
        if (millis() - alteZeit > 999) {
            alteZeit = millis();
            lcd.setCursor(0, 0);
            lcd.print("Initialisierung: ");
            if (i >= 10) {
                    lcd.print(i);
            } else {
                    lcd.print(" ");
                    lcd.print(i);
            }
            i --;
        }
    }

    delay(1000);
    lcd.clear();
}


void loop() {
    // write your code here

    // Debugging Mode extern forcieren
    if (PIN5 == LOW) {
        debuggingMode = true;
    } else {
        debuggingMode = softwareDebuggingMode;
    }

    bme680.performReading();

    //bool statusVerbindung = netzwerkTest();
    float temperatur = bme680.readTemperature();
    float feuchtigkeit = bme680.readHumidity();
    float luftdruck = bme680.readPressure()/100000;

    static unsigned long zeitSenden = 0;


    // Ausgabe auf Display
    anzeigeDisplay(temperatur, feuchtigkeit, luftdruck);

    // delay(10000);
    // Alle 60 Sekunden Daten an ThingSpeak senden
    if (millis() - zeitSenden > 59999) {
        zeitSenden = millis();

        sendBME680Data(temperatur, feuchtigkeit, luftdruck);
    }


    // delay(10000);

}

void verbindungWlan() {

    lcd.setCursor(0, 2);
    lcd.print("Status WLAN:        ");

    if(!sendAT("AT","OK",5000)) {
        lcd.setCursor(0,3);
        lcd.print("Keine Verbindung    ");
        return;
    }
    if(!sendAT("AT+CWMODE=1","OK",5000)) {
        lcd.setCursor(0,3);
        lcd.print("Fehler im WLAN Modus");
        return;
    }

    String befehl = String("AT+CWJAP=\"") + ssid + "\",\"" + password + "\"";
    if (!sendAT(befehl.c_str(), "WIFI CONNECTED", 45000)) {
        lcd.setCursor(0,3);
        lcd.print("WLAN nicht verbunden");
        return;
    }

    lcd.setCursor(0,3);
    lcd.print("WLAN Verbunden      ");
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
                if (debuggingMode == true) {
                    lcd.setCursor(0,2);
                    lcd.print("Antwort WLAN-Modul: ");
                    lcd.setCursor(0,3);
                    lcd.print(antwort);
                }
                return true;
            }
        }
    }

    lcd.setCursor(0,3);
    lcd.print("Timeout AT command  ");
    return false;
}

// Funktion Daten an Server senden
void sendBME680Data(float temperature, float humidity, float pressure) {
    // LCD-Ausgabe mit Überprüfung der Sensorwerte
    // Wird für Debugging verwendet
    if (debuggingMode == true) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("T: ");
        lcd.print(temperature, 2);
        lcd.setCursor(0, 1);
        lcd.print("H: ");
        lcd.print(humidity, 2);
        lcd.setCursor(0, 2);
        lcd.print("P: ");
        lcd.print(pressure, 2);
    }
    lcd.setCursor(0, 3);
    lcd.print("Sende Daten...      ");

    if (debuggingMode == true) {
        // Bei Debugging Wert auf 2000 anpassen
        delay(2000);
    } else {
        delay(4000);
    }

    // Prüfung ob URL und API korrekt übernommen werden
    // Wird für Debugging verwendet
    if (debuggingMode == true) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(url_bme680); // Erscheint auf Zeile 1 und 3, da zu Faul Zeichen vom Zeiger umzuwandeln
        lcd.setCursor(0, 3);
        lcd.print(api_key_bme680);

        delay(2000);
    }

    // Prüfung, ob die Sensorwerte sauber übernommen wurden
    if (isnan(temperature) || isnan(humidity) || isnan(pressure)) {
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("  Daten von BME680  ");
        lcd.setCursor(0, 2);
        lcd.print(" sind nicht valide! ");
        return;
    }



    // Verbindung zum Server herstellen
    if (!sendAT("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80", "CONNECT", 5000)) {
        lcd.setCursor(0, 3);
        lcd.print("No Server connection");
        return;
    } else {
        lcd.setCursor(0, 3);
        lcd.print("Server connection ok");
    }

    delay(1000);

    // HTTP-GET-Request vorbereiten
    // Get Request einzeln aufbauen, um Fehler in der Funktion zu minimieren
    String getRequest = "GET /update?api_key=";
    getRequest += api_key_bme680;
    getRequest+= "&field1=";
    getRequest+= String(temperature, 2);
    getRequest+= "&field2=";
    getRequest+= String(humidity, 2);
    getRequest+= "&field3=";
    getRequest+= String(pressure, 2);
    getRequest+= " HTTP/1.1\r\nHost: api.thingspeak.com\r\nConnection: close\r\n\r\n";

    // Ausgabe der Adresse. Sehr schlecht auf 2004 LCD zu lesen
    // Wird für Debugging verwendet
    if (debuggingMode == true) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(getRequest);
    }

    delay(1000);

    // Unterer Teil wird für Debugging verwendet
    if (getRequest.length() == 0) {
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("!!!Request ERROR!!! ");
        lcd.setCursor(0, 2);
        lcd.print("  String is Empty   ");
        return;
    } else if (debuggingMode == true) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("HTTP Request:       ");
        lcd.setCursor(0, 1);
        lcd.print(getRequest);
        lcd.setCursor(0, 2);
        lcd.print("Request length:     ");
        lcd.setCursor(0, 3);
        lcd.print(getRequest.length());
    }


    delay(1000);

    // Länge des Requests berechnen und senden
    String cipsendCommand = String("AT+CIPSEND=") + getRequest.length();

    if (!sendAT(cipsendCommand.c_str(), ">", 10000)) {
        lcd.setCursor(0, 3);
        lcd.print("CIPSEND ERROR       ");
        return;
    } else {
        lcd.setCursor(0, 3);
        lcd.print("CIPSEND OK          ");
    }

    delay(1000);

    // Sende den HTTP-Request
    Serial.print(getRequest);
    lcd.setCursor(0, 3);
    lcd.print("sending request     ");

    delay(1000);

    // Warten auf die Antwort
    if (!sendAT("","SEND OK",10000)) {
        lcd.setCursor(0, 3);
        lcd.print("SEND ERROR       ");
        return;
    }


    // Verbindung schliessen, falls nicht automatisch geschehen
    // Falls die Verbindung schon geschlossen ist, gibt der erneute Aufruf ein Timeout zurück
    delay(1000);

    if (!sendAT("AT+CIPCLOSE","ClOSED",5000)) {
        lcd.setCursor(0, 3);
        lcd.print("CLOSE ERROR         ");
    }

    if (debuggingMode == true) {
        delay(3000);
    }

    lcd.setCursor(0, 3);
    lcd.print("Daten gesendet!  :) ");

}

// Funktion Displayanzeige
void anzeigeDisplay(float temperatur, float feuchtigkeit, float luftdruck) {
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
    //lcd.print();
}
