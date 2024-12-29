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

// Anschluss SIM7600g-h Modul definieren
//SoftwareSerial sim7600g(11,10);

// ThingSpeak API Keys
const char* api_key_bme680 = "VFNZDUII0ENDF526";  // Sicherheitsrisiko, da API Key unverschlüssselt. in einer späteren Version anzupassen

// URL Thingspeak
const char* url_bme680 = "http://api.thingspeak.com/update";


// Definition Funktion Daten senden
void sendBME680Data(float temperature, float humidity, float pressure);

// Definition Funktion 2004 Display
void anzeigeDisplay(float temperatur, float feuchtigkeit, float luftdruck, bool statusVerbindung);

bool netzwerkTest ();

void setup() {
// write your initialization code here

    //sim7600g.begin(19200);
    Wire.begin();


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
    bme680.begin(BME680_I2C_ADDRESS);

// Geschwindigkeit der seriellen Verbindung definieren
   Serial.begin(19200);
}

void loop() {
    // write your code here
    bme680.performReading();

    bool statusVerbindung = netzwerkTest();
    float temperatur = bme680.readTemperature();
    float feuchtigkeit = bme680.readHumidity();
    float luftdruck = bme680.readPressure()/100000;

    // Ausgabe auf Display
    anzeigeDisplay(temperatur, feuchtigkeit, luftdruck, statusVerbindung);

    delay(10000);


    // Daten an ThingSpeak senden
    sendBME680Data(temperatur, feuchtigkeit, luftdruck);


    delay(10000);

}

// Funktion Daten an Server senden
void sendBME680Data(float temperature, float humidity, float pressure) {
    String url = String(url_bme680) + "?api_key=" + api_key_bme680 +
                 "&field1=" + String(temperature) +
                 "&field2=" + String(humidity) +
                 "&field3=" + String(pressure);

    // APN für Netzzugang in Modem setzen
    //sim7600g
    Serial.println("AT+CGDCONT=1,\"IP\",\"gprs.swisscom.ch\"");
    delay(500);
    //sim7600g
    Serial.println("AT+CGATT=1");
    delay(500);
    //sim7600g
    Serial.println("AT+CGACT=1,1");
    delay(500);
    //sim7600g
    Serial.println("AT+HTTPPARA=\"CID\",1");
    delay(500);

    // Eigentlicher HTTP aufruf
    //sim7600g
    Serial.println("AT+HTTPINIT");
    delay(500);
    //sim7600g
    Serial.println("AT+HTTPPARA=\"URL\",\"" + url + "\"");
    delay(500);
    //sim7600g
    Serial.println("AT+HTTPACTION=0");
    delay(3000);
    //sim7600g
    Serial.println("AT+HTTPTERM");
}

// Funktion Displayanzeige
void anzeigeDisplay(float temperatur, float feuchtigkeit, float luftdruck, bool statusVerbindung) {
    lcd.clear();
    // Zeile 1
    lcd.setCursor(0, 0);
    lcd.print("Temperatur: ");
    lcd.print(temperatur);
    lcd.print(" C");
    // Zeile 2
    lcd.setCursor(0, 1);
    lcd.print("Feuchtigkeit: ");
    lcd.print(feuchtigkeit);
    lcd.print(" %");
    // Zeile 3
    lcd.setCursor(0, 2);
    lcd.print("Luftdruck: ");
    lcd.print(luftdruck);
    lcd.print(" bar");
    // Zeile 4
    lcd.setCursor(0, 3);
    /*if (statusVerbindung) {
        lcd.print("Verbindung gut");
    }
    else {
        lcd.print("Verbindung nicht gut");
    }*/
    lcd.print(statusVerbindung);
}

// Funktion Verbindungsstatus testen
bool netzwerkTest () {
    String antwort ="";
    //sim7600g
    Serial.println("AT+CREG?");
    delay(500);
    while (/*sim7600g*/Serial.available()) {
        char c = /*sim7600g*/Serial.read();
        antwort += c;
    }

    /*lcd.clear();
    lcd.setCursor(0, 0);
    if (antwort.length() > 20) {
        lcd.print(antwort.substring(0, 20));
        lcd.setCursor(0, 1);
        lcd.print(antwort.substring(20, 40));
    } else {
        lcd.print(antwort);
    }*/

    int antwortpositiv = antwort.indexOf("+CREG:");  // Prüfen, ob die Antwort vom SIM7600g-h positiv ist
    if (antwortpositiv != -1) {
        // Antwort nach dem letzten Komma auswerten. ,1 bedeutet: regristriert im Heimnetzwerk. ,5 bedeutet: regristriert, roaming.
        int letztesKomma = antwort.lastIndexOf(',');
        if (letztesKomma != -1 && (letztesKomma +1) < (int)antwort.length()) {
            char status = antwort[letztesKomma + 1];
            if ( status == '1' || status == '5') {
                return true;
            }
        }
    }
    return false;
}
