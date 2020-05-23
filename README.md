# P1Meter-EmonCMS
Arduino Sketch om data uit de P1-poort van de digitale electiciteitsmeter te uploaden naar EmonCMS

## Gebruikte hardware: 
* Wemos D1 mini V2 ESP8266
* RJ12 plug, BS170 mosfet
* 5V rails power supply

Aan de P1-uitgang stroommeter hangt een Wemos D1 mini V2. Deze is ingebouwd in een module in de meterkast. Deze ESP8266 geeft via Wifi de metingen door naar EmonCMS. 

## Electronica
De P1 aansluiting heeft een RJ-12 port. Daarvan zijn 3 pinnen in gebruik, zie het schema hier: http://domoticx.com/p1-poort-slimme-meter-hardware De pinnen die je gebruikt zijn: Data Request(2), Data(5) en Data GND(3). Pin 1 en 6 zijn niet in gebruik dus een RJ-11 telefoonkabel werkt prima (4 aders).
* Van de Wemos D1 gebruik ik de pinnen: +5V, GND, +3V3, D1 en D6. 
* D6 wordt gebruikt om Data Request te schakelen (aan/uit). Data Request is via een 220 ohm weerstand verbonden met +5V. Via een BS-170 mosfet wordt deze omlaag getrokken naar GND als D6 aan gaat.
* D1 wordt aangesloten op de Data pin. Ook wordt een weerstand gelegd tussen +3V3 en D1, zodat D1 hoog is als de Data pin geen GND is. D1 is geinverteerd en wordt via SoftSerial uitgelezen.
* Een 5V voeding wordt aangesloten op +5V en GND. Deze heeft een aparte modulaire behuizing die in de stoppenkast past. De Wemos D1 zit met een RJ-12 plug in een aparte module.


