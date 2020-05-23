# P1Meter-EmonCMS
Arduino Sketch om data uit de P1-poort van de digitale electiciteitsmeter te uploaden naar EmonCMS

## Gebruikte hardware: 
* [Wemos D1 mini V2 ESP8266](wemos_d1miniV2.jpg)
* RJ12 plug, BS170 mosfet
* [5V DIN rail power supply](https://nl.farnell.com/mean-well/hdr-15-5/power-supply-ac-dc-5v-2-4a/dp/2815635)
* [DIN rail enclosure](https://nl.farnell.com/camdenboss/cmeb/enclosure-din-grey-extendable/dp/1774480)

Aan de P1-uitgang stroommeter hangt een Wemos D1 mini V2. Deze is ingebouwd in een module in de meterkast. Deze ESP8266 geeft via Wifi de metingen door naar EmonCMS. 

## Electronica
De P1 aansluiting heeft een RJ-12 port. Daarvan zijn 3 pinnen in gebruik, zie het schema [hier](http://domoticx.com/p1-poort-slimme-meter-hardware). De pinnen die je gebruikt zijn: Data Request(2), Data(5) en Data GND(3). Pin 1 en 6 zijn niet in gebruik dus een RJ-11 telefoonkabel werkt prima (4 aders). Zie [pin-layout](ESMRv5.0-P1-schematic.png).
* Van de Wemos D1 gebruik ik de pinnen: +5V, GND, +3V3, D1 en D6. 
* D6 wordt gebruikt om Data Request te schakelen (aan/uit). Data Request is via een 220 ohm weerstand verbonden met +5V. Via een BS-170 mosfet wordt deze omlaag getrokken naar GND als D6 aan gaat.
* D1 wordt aangesloten op de Data pin. Ook wordt een weerstand gelegd tussen +3V3 en D1, zodat D1 hoog is als de Data pin geen GND is. D1 is geinverteerd en wordt via SoftSerial uitgelezen.
* Een 5V voeding wordt aangesloten op +5V en GND. Deze heeft een aparte modulaire behuizing die in de stoppenkast past. De Wemos D1 zit met een RJ-12 plug in een aparte module.

[Schets van de aangesloten electronica](T1Schema.jpg)

## Links
* EmonCMS: https://openenergymonitor.org/
* EmonCMS upload example: http://domoticx.com/p1-poort-slimme-meter-data-naar-emoncms-arduino/
* P1 port: http://domoticx.com/p1-poort-slimme-meter-hardware/#
* Domoticz: https://www.domoticz.com/wiki/Main_Page
* Domoticz P1 upload: http://domoticx.com/p1-poort-slimme-meter-data-naar-domoticz-esp8266/
* Arduino aan de P1 port: https://www.circuitsonline.net/forum/view/106111
* Alternatief voor EmonCCMS: DSMR-reader: https://github.com/dennissiemensma/dsmr-reader

