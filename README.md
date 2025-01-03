# Projektarbeit Arduino

## Inhalt
- [Ausgangslage](#ausgangslage)
- [Anforderungen](#anforderungen)
- [Flussdiagramm](#flussdiagramm)
- [Lösungansatz](#lösungsansatz)
  - [Probleme](#probleme)
    - [SIM7600G-H Modu](#sim7600g-h-modul)
    - [ESP01S](#esp01s)
    - [BME680 Sensor](#bme680-sensor)
    - [Arduino Nano](#arduino-nano)
    - [ThingSpeak](#thingspeak)
  - [Lösung](#lösung)
- [Eingesetztes Material](#eingesetztes-material)
- [Funktionstest](#funktionstest)
- [Video](#video)
  - [Normaler Modus](#normaler-modus)
  - [Debugging Modus](#debugging-modus)
- [Quellenangabe](#quellenangabe)
  - [Dokumente](#dokumente)
  - [Webseiten](#webseiten)
  - [Youtube Videos](#youtube-videos)

## Ausgangslage
In meinem Hobbyraum sind die Umweltbedinungen nicht immer ideal.
Trotz mehrerer fest installierter Heizkörper erreicht der Raum im Winter selten Temperaturen über 19°C.
Ebenfalls ist die Luft sehr trocken. Da ich in meinem Hobbyraum zwei 3D Drucker stehen habe, ist mir die Überwachung der Drucker wichtig.
Dazu benötige ich die Umweltdaten des Raums und wenn möglich ein Bild des Druckkopfs. Mit dem Projekt in Informatik möchte ich mir die benötigten Grundlagen aneignen, um dies später in einer verbeserten Version funktionstüchtig zu haben.

## Anforderungen
|Anforderungen|Muss|Wunsch|
|----------------------|----|----|
|Verbindungsaufbau mit zweitem Gerät.| x ||
|Verbindung mittels 4G Modul SIM7600G-H, Bluetooth Modul DX-BT24 oder WLAN Modul ESP01S aufbauen.| x ||
|Daten von Arduino an Empfangsgerät übertragen.| x ||
|Sinnvolle Anzeige von aktuellem Betriebszustand des Arduinos auf Display mittels BME680 (Lufttemperatur, relative Luftfeuchtigkeit und barometrischer Druck).| x ||
|Steuerung von Ausgängen an Arduino mittels verbundenen Geräts (Relaiskarte).|| x |
|Kamerabild (Kamera OV7670) aufnehmen und auf Empfangsgerät anzeigen.|| x |

## Flussdiagramm
kkkkkk

## Lösungsansatz
Ich möchte mit einem Arduino Nano einen BME680 Sensor von Bosch auslesen und die Daten dann auf Thingspeak hochladen.
Dazu habe ich einen BME680 mittels I2C an meinen Arduino Nano angeschlossen.
Mittels einem 2004 LCD sehe ich jedeerzeit die aktuelle Raumluft, die aktuelle Luftfeuchtigkeit und den aktuellen barometrischen Luftdruck.
Das LCD wird ebenfalls per I2C angeschlossen.
Die Daten werden entweder mittels einem SIM7600G-h Modem oder einem ESP01S WLAN-Modul ins Internet auf die Plattform Thingspeak geladen.
Der aktuelle Programmschritt, oder allfällige Fehler während dem Senden werden ebenfalls auf dem Display angezeigt.

### Probleme
Beim Projekt traten mehrere Probleme auf. Nicht alle davon konnten zufriedenstellend gelöst werden.
Folgende Probleme traten auf:

#### SIM7600G-H Modul
Das SIM7600G-H Modul scheint einen technischen defekt, oder eine fehlerhafte Firmware zu besitzen. Folgende Fehler traten am Modul auf, welche mich zu dieser Annahme verleiten.
- Die Verbindung SIM7600G-H zu Arduino funktionierte weder über Software Serial, noch Hardware Serial
- Das SIM7600G-H Modul speichert Einstellungen wie zum Beispiel Baudrate nicht. Dies obwohl es auf den Speicherbefehl mit ok antwortet.
- Einzelne AT Commands werden teilweise nicht ausgeführt. Dies obwohl sie gemäss Herstellerdokumentation die, für diese Firmware Version, passenden Commands wären.
- Zeitweise friert das Modul einfach ein und nur ein Hardware-Reset löst das Problem. Dieses einfrieren kann zu jeder Zeit erfolgen. Ich war nicht in der Lage, eine Ursache zu finden.
Aufgrund des fehlerhaften verhaltens des SIM7600G-H Moduls wurde entschieden, dieses nicht zu verwenden und statt dessen auf ein WLAN Modul zu wechseln.

#### ESP01S
- Das ESP01S reagiert extrem sensibel auf Spannungsschwankungen. Ein Arduino ist nicht in der Lage stabile 3.3V auszugeben, weshalb ein externes Netzteil benötigt wird.
- Der ESP01S ist nach der Ausführung eines AT Commands nicht in der Lage sofort einen neuen Befehl auszuführen. Je nach Befehl wird unterschiedlich viel Zeit benötigt, bis der Controller wieder Ansprechbar ist.

#### BME680 Sensor
Der Sensor benötigt zwischen 3.3 bis 5 Volt Betriebsspannung und einen 3.3V Bus. Gemäss dem Hersteller des Sensormoduls geht ein normaler 5V Bus. Dies ist aber ganz klar nicht der Fall. Bei einem 5V Bus ist der Sensor nicht ansprechbar. Es wird dementsprechend ein Level Converter benötigt.

#### Arduino Nano
Der Arduino hat mehrere Probleme, welche aber seinem Alter als Mikrocontroller geschuldet sind.
Der Arduino kann kein keinen stabilen Software Serial mit einer Baudrate höher als 38400 Baud erzeugen. Bei einer Baudrate von 57600 Baud und höher, wird der Bus instabil.
Der Hardware Serial ist stabiler. Ab einer Baudrate von mehr als 115200 Baud muss mit verlorenen Daten gerechnet werden, bzw eine Funtion implementiert werden, die dies abfängt.
Ein weiteres Problem des Arduinos ist seine Leistungsfähigkeit im allgemeinen. Im Projekt habe ich einen etwas längeren String gehabt, bei welchem der Arduino die Bytes hätte zählen müssen. Dazu war der Arduino nicht in der Lage. Der String musste in einzelne Snippets zerteilt werden und nach und nach die Bytes gezählt und aufsummiert werden. 
Aufgrund des begrenzten Speichers, des begrenzten RAM und der geringen Taktrate ist der Arduino niocht in der Lage, dass Bild einer OV7670 Kamera auszulesen und als Bild abzuspeichern. Das Bild muss als Rohdaten in einem externen Speicher zwischengespeichert werden und dann mit einem externen Programm, zB auf einer Website, in ein JPEG umgewandelt werden.

#### ThingSpeak
Ohne kostenpflichtiges Abo ist ThingSpeak nur sehr begrenzt nutzbar. Bilddaten können zum Beispiel nur mit einem Abo hochgeladen werden.
Mit kostenpflichtigem Abo können zwar Bilder hochgeladen werden, Jedoch sind pro Datei nur 5MB zulässig und das Bild muss als JPEG vorliegen. Rohdaten, zB von meinem Arduino, können nicht verarbeitet werden.

### Lösung
Die Aufgabenstellung wurde in mehrere Unterkategoprien einegteilt. Zuerst wurde Das 2004 I2C LCD implementiert. Die Ursprüngliche Library von Adafruit wurde wegen inkompatibiltäten verworfen und stattdessen die mir schon bekannte Library von Marco Schwartz verwendet.

Aufgrund der Fehlinformation über die Busspannung am Bosch BME680 Sensormodul von Waveshare, wurde der Fehler lange Zeit im Code, beziehungsweise der Library von Adafruit gesucht. Dank ChatGPT kam ich auf die Lösung, dass der Arduino Nano eine I2C Busspannung von 5V liefert, der Sensor aber nur mit 3,3V Busspannung umgehen kann. Die meine Lösung für das Problem, war ein bidirektionaler Level-Shifter, welchen ich noch herumliegen hatte. Da ich mehrere verschiedene Level Shifter in meinem Fundus habe, konnte ich mehrere Level-Shifter austesten und habe mich dann aufgrund der Möglichkeit, mehrere unterschiedliche Signale getrennt zu shiften für einen No-Name 8 Kanal Level-Shifter entschieden. Da ich nicht wusste, ob der eingesetzte Sensor von Waveshare noch funktionierend war, habe ich für den Funktionstest mit dem Level-Shifter ein CJMCU680 Modul eingesetzt, welches ein chinesisches Knock-Off des BME680 Sensors ist. Nachdem dies funktioniert hat, konnte ich feststellen, ob der BME680 Sensor ebenfalls funktioniert. Dies war der Fall, weshalb ich diesen auch weiterhin einsetze.

Das SIM7600G-H Modem hat mir am meisten Probleme bereitet. Es war lange nicht klar, weshalb die Kommunikation nicht funktioniert. Erst ausgiebige Tests am PC mit Hilfe von PuTTY und ChatGPT, haben ergeben, dass das Modem fehlerhaft ist. Wie schon im Unterkapitel Probleme erwähnt, wurde darauf hin entschieden das Modem zu ersetzen.
Als Ersatz kam ein ESP01S zum Einsatz, welcher als WLAN Modul eingesetzt wurde.

Der ESP01S hat wie auch der BME680 eine 3.3V Schnittstelle. Da ich schon für den I2C Bus einen 8 Kanal Level Shifter im Einsatz habe, konnte ich einfach einen anderen Kanal für die serielle Kommunikation nutzen.
Etwas komplizierter war es die Spannungsversorgung sicherzustellen. Der ESP01S benötigt eine stabile 3,3V Spannungsversorgung. Der Arduino besitzt zwar einen 3,3V Ausgang, dieser ist jedoch niocht stabilisiert. Als Lösung habe ich vom HW-140 DC-DC Converter, welcher schon die 3,3V für den Level-Shifter liefert, eine Leitung abgezweigt.
Das der ESP01S nach dem Ausführen eines AT Commands nicht sofort wieder Empfangsbereit ist, führte zu einem Timing Problem. Die Reihenfolge und das Timing der AT Commands musste perfektioniert werden. Um sicher zu gehen, dass der Arduinonicht aus versehen etwas anderes ausführt, wurde beim Senden auf die Funktion millis() verzichtet und stattdessen delay() eingesetzt. Die korrekten Wartezeiten mussten via Trial and Error eruiert werden. Jegliche Manipulatuion am Code kann hier zu einem unerwarteten Ergebnis führen. Vor dem Senden der Daten, wollte ich eine überprüfung des WLAN Statuses durchführen und gegebenenfalls die Verbindung erneut aufbauen. dies führte jedoch ale paar Zyklen zu einem CIPSEND Error. aus diesem Grund wurde im endgültigen Projekt auf die Überprüfung der Netzwerkverbindung verzichtet und nur ein Ansatz für eine spätere Version als auskommentierten Code im Main-File gelassen.



## Eingesetztes Material
Für die Lösung wurde mehreres Material eingesetzt. Nicht alles davon hat es in die endgültige Lösung geschafft. In der Das eingesetzte Material und dessen Verwendung ist in der Nachfolgenden Tabelle gelistet.
| Material | Geplannt | Ungeplannt | Final eingesetzt | Verworfen wegen Problemen |
|----------|----------|------------|------------------|---------------------------|
|Arduino Nano|x||x||
|Bosch BME680|x||x||
|2004 I2C LCD|x||x||
|SIM7600G-H Modem|x|||x|
|ESP01S WLAN Modul||x|x||
|8 Ch Level Shifter||x|x||
|RTC DS1302|x|||x|
|externes Netzteil||x|x||
|HW-140 DC-DC Converter||x|x||
|OV7670 Kamera|x|||x|
|16 Mb Flash Speicher||x|||

## Funktionstest
sfdsfsd

## Video
## Normaler Modus
dfsg

## Debugging Modus
fggg

## Quellenangabe
### Dokumente
[RTC Modul](/AZ219_C8-4_DE_B07WS84H44.pdf)

[SIM7600G-H AT Commands](/SIM7500_SIM7600 Series_AT_Command_Manual_V3.00.pdf)

### Webseiten
https://www.waveshare.com/wiki/SIM7600G-H_4G_Module#Datasheet

https://techship.com/downloads/simcom-sim7600g-h-firmware-le20b03sim7600m22-binary-files-archive-and-release-notes/

https://momoiot.co.kr/lte-at/module/diagnostic/

https://momoiot.co.kr/lte-at/module/sim7600-driver/

https://momoiot.co.kr/lte-at/module/qdl/

https://momoiot.co.kr/lte-at/module/qdl-manual-7600/

https://momoiot.co.kr/lte-at/module/diagnostic/

https://www.multitech.net/developer/downloads/

https://www.instructables.com/A-Quick-Guide-on-Logic-Level-Shifting/

https://www.mikrocontroller.net/articles/Pegelwandler

https://www.waveshare.com/wiki/SIM7600G-H_4G_Module#Resource

https://www.waveshare.com/wiki/SIM7600G-H_4G_HAT_(B)

https://adafruit.github.io/Adafruit_BME680/html/class_adafruit___b_m_e680.html

https://www.waveshare.com/wiki/BME680_Environmental_Sensor#Working_with_Arduino

https://www.waveshare.com/wiki/Bme680#For_Arduino

https://docs.arduino.cc/language-reference/en/functions/digital-io/pinMode/

https://docs.arduino.cc/language-reference/en/functions/communication/SPI/

https://docs.arduino.cc/language-reference/en/functions/communication/stream/

https://docs.arduino.cc/language-reference/en/functions/communication/Serial/

https://docs.arduino.cc/language-reference/en/functions/communication/Print/

https://docs.arduino.cc/language-reference/en/functions/communication/Wire/

https://docs.arduino.cc/learn/communication/wire/

https://wiring.org.co/learning/tutorials/bluetooth/

https://www.az-delivery.de/en/blogs/azdelivery-blog-fur-arduino-und-raspberry-pi/hc-05-bluetooth-modul-einfuhrung

https://randomnerdtutorials.com/bme680-sensor-arduino-gas-temperature-humidity-pressure/

https://www.waveshare.com/wiki/SIM7600G-H_4G_Module

https://forum.arduino.cc/t/waveshare-sim7600g-h-at-commands-on-serial-port/1118316

https://www.az-delivery.de/blogs/azdelivery-blog-fur-arduino-und-raspberry-pi/esp-easy-mit-thingspeak

https://www.az-delivery.de/blogs/azdelivery-blog-fur-arduino-und-raspberry-pi/pause-muss-auch-mal-sein-beispiele-zum-nichtblockierenden-programmablauf

https://docs.arduino.cc/language-reference/de/funktionen/time/millis/

https://pressbooks.pub/sloiot/chapter/beispiel-5/

https://www.instructables.com/Step-by-Step-Guide-to-Sending-SMS-With-SIM7600-a-C/

https://www.tutorials.at/c/11-zeiger.html

https://docs.arduino.cc/learn/communication/wire/

https://www.waveshare.com/wiki/Bme680#Software

https://learn.sparkfun.com/tutorials/bi-directional-logic-level-converter-hookup-guide/all


### Youtube Videos
https://www.youtube.com/watch?v=m4aR494k3v8

https://www.youtube.com/watch?v=vS3-WbvKsKE

https://www.youtube.com/watch?v=kHxQNR6nzkI

https://www.youtube.com/watch?v=9rQc4b5fNMc
