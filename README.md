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
- [Funktionstest](#funktionstest)
- [Video](#video)
  - [Normaler Modus](#normaler-modus)
  - [Debugging Modus](#debugging-modus)
- [Quellenangabe](#quellenangabe)
  - [Dokumente](#dokumente)
  - [Webseiten](#webseiten)
  - [Youtube Videos](#youtube-videos)

## Ausgangslage
In meinem Hobbyraum sind Umweltbedinungen nicht immer ideal.
Trotz mehrerer fest installierter Heizkörper erreicht der Raum im Winter selten Temperaturen über 19°C.
Ebenfalls ist die Luft sehr trocken. Da ich in meinem Hobbyraum zwei 3D Drucker stehen habe, ist mir die Überwachung der Drucke wichtig.
Dazu benötige ich die Umweltdaten des Raums und wenn möglich ein Bild des Druckkopfs. Mit dem Projekt in Informatik, möchte ich mir die benötigten Grundlagen aneignen, um dies später in einer verbeserten Version lauffähig zu haben.

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
Die Daten werden entweder mittels einem SIM7600G-h Modem, oder einem ESP01S WLAN-Modul ins Internet auf die Plattform Thingspeak geladen.
Der aktuelle Programmschritt, oder allfällige Fehler während dem Senden werden ebenfalls auf dem Display angezeigt.

## Probleme
Beim Projekt traten mehrere Probleme auf. Nicht alle davon konnten zufriedenstellend gelöst werden.
Folgende Probleme traten auf:
### SIM7600G-H Modul
Das SIM7600G-H Modul scheint einen technischen defekt, oder eine fehlerhafte Firmware zu besitzen. Folgende Fehler traten am Modul auf, welche mich zu dieser Annahme verleiten.
- Die Verbindung SIM7600G-H zu Arduino funktionierte weder über Software Serial, noch Hardware Serial
- Das SIM7600G-H Modul speichert Einstellungen wie zum Beispiel Baudrate nicht. Dies obwohl es auf den Speicherbefehl mit ok antwortet.
- Einzelne AT Commands werden teilweise nicht ausgeführt. Dies obwohl sie gemäss Herstellerdokumentation die, für diese Firmware Version, passenden Commands wären.
- Zeitweise friert das Modul einfach ein und nur ein Hardware-Reset löst das Problem. Dieses einfrieren kann zu jeder Zeit erfolgen. Ich war nicht in der Lage, eine Ursache zu finden.

### ESP01S
- Das ESP01S reagiert extrem sensibel auf Spannungsschwankungen. Ein Arduino ist nicht in der Lage stabile 3.3V auszugeben, weshalb ein externes Netzteil benötigt wird.
- Der ESP01S ist nach der Ausführung eines AT Commands nicht in der Lage sofort einen neuen Befehl auszuführen. Je nach Befehl wird unterschiedlich viel Zeit benötigt, bis der Controller wieder Ansprechbar ist.

### BME680 Sensor
Der Sensor benötigt zwischen 3.3 bis 5 Volt Betriebsspannung und einen 3.3V Bus. Gemäss dem Hersteller des Sensormoduls geht ein normaler 5V Bus. Dies ist aber ganz klar nicht der Fall. Bei einem 5V Bus ist der Sensor nicht ansprechbar. Es wird dementsprechend ein Level Converter benötigt.

### Arduino Nano
Der Arduino hat mehrere Probleme, welche aber seinem Alter als Mikrocontroller geschuldet sind.
Der Arduino kann kein keinen stabilen Software Serial mit einer Baudrate höher als 38400 Baud erzeugen. Bei einer Baudrate von 57600 Baud und höher, wird der Bus instabil.
Der Hardware Serial ist stabiler. Ab einer Baudrate von mehr als 115200 Baud muss mit verlorenen Daten gerechnet werden, bzw eine Funtion implementiert werden, die dies abfängt.
Ein weiteres Problem des Arduinos ist seine Leistungsfähigkeit im allgemeinen. Im Projekt habe ich einen etwas längeren String gehabt, bei welchem der Arduino die Bytes hätte zählen müssen. Dazu war der Arduino nicht in der Lage. Der String musste in einzelne Snippets zerteilt werden und nach und nach die Bytes gezählt und aufsummiert werden. 
Aufgrund des begrenzten Speichers, des begrenzten RAM und der geringen Taktrate ist der Arduino niocht in der Lage, dass Bild einer OV7670 Kamera auszulesen und als Bild abzuspeichern. Das Bild muss als Rohdaten in einem externen Speicher zwischengespeichert werden und dann mit einem externen Programm, zB auf einer Website, in ein JPEG umgewandelt werden.

### ThingSpeak
Ohne kostenpflichtiges Abo ist ThingSpeak nur sehr begrenzt nutzbar. Bilddaten können zum Beispiel nur mit einem Abo hochgeladen werden.
Mit kostenpflichtigem Abo können zwar Bilder hochgeladen werden, Jedoch sind pro Datei nur 5MB zulässig und das Bild muss als JPEG vorliegen. Rohdaten, zB von meinem Arduino, können nicht verarbeitet werden.

## Funktionstest
sfdsfsd

## Video
## Normaler Modus
dfsg

## Debugging Modus
fggg

## Quellenangabe
### Dokumente
dfgdfgd

### Webseiten
eesgd
### Youtube Videos
fsse
