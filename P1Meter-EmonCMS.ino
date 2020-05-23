/*
  -------------How to wire Adafruit Huzzah or Wemos D1 mini V2 to Smart meter using RJ11 cable-------------
  Yellow to pin D1
  Red to ground
  Base of BS170 to pin D6
  Collector of BS170 to V+ (=5V) via 220 ohm resistor
  Black to collector pin of BS170
  Emitter of BS170 to GND
  1K resistor between 3V3 and pin D1

  -------------Example P1 telegram----------------------------
  /XMX5LGBBLB1232323126       Manufacturer specific header

  1-3:0.2.8(50)               This meter complies to DMSR 5.0
  0-0:1.0.0(200517214933S)    Timestamp
  0-0:96.1.1(4530303336303033383336343934333137) Serial number
  1-0:1.8.1(005163.777*kWh)   Tariff 1 Meter Reading
  1-0:1.8.2(005413.811*kWh)   Tariff 2 Meter Reading
  1-0:2.8.1(001023.095*kWh)   Tariff 1 delivered by client
  1-0:2.8.2(001843.428*kWh)   Tariff 2 delivered by client
  0-0:96.14.0(0001)           current Tariff
  1-0:1.7.0(00.413*kW)        Actual energy delivered
  1-0:2.7.0(00.000*kW)        Actual energy receiverd
  0-0:96.7.21(00012)          Number of power failures in any phase
  0-0:96.7.9(00000)           Number of long power failures in any phase
  1-0:99.97.0(0)(0-0:96.7.19) Power failure event log
  1-0:32.32.0(00006)          Number of voltage sags in phase L1
  1-0:52.32.0(00006)          Number of voltage sags in phase L2
  1-0:72.32.0(00003)          Number of voltage sags in phase L3
  1-0:32.36.0(00000)          Number of voltage swells in phase L1
  1-0:52.36.0(00000)          Number of voltage swells in phase L2
  1-0:72.36.0(00000)          Number of voltage swells in phase L3
  0-0:96.13.0()               Text message codes: numeric 8digits
  1-0:32.7.0(229.0*V)         Instantaneous voltage L1
  1-0:52.7.0(228.0*V)         Instantaneous voltage L2
  1-0:72.7.0(229.0*V)         Instantaneous voltage L3
  1-0:31.7.0(000*A)           Instantaneous current L1 in A resolution.
  1-0:51.7.0(000*A)           Instantaneous current L2 in A resolution.
  1-0:71.7.0(001*A)           Instantaneous current L3 in A resolution.
  1-0:21.7.0(00.023*kW)       Instantaneousactive power L1 (+P)
  1-0:41.7.0(00.041*kW)       Instantaneousactive power L2 (+P)
  1-0:61.7.0(00.347*kW)       Instantaneousactive power L3 (+P)
  1-0:22.7.0(00.000*kW)       Instantaneousactive power L1 (-P)
  1-0:42.7.0(00.000*kW)       Instantaneousactive power L1 (-P)
  1-0:62.7.0(00.000*kW)       Instantaneousactive power L1 (-P)
  0-1:24.1.0(003)             Auxilary device type (3 = gas meter)
  0-1:96.1.0(1231231231231231231231231231231231) Auxilary device equipment identifier
  0-1:24.2.1(200517214507S)(03842.527*m3) Gas meter readout
  !8901                       Telegram CRC value
*/
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <WiFiClientSecure.h>
#include <time.h>
#include "MeterP1.h"
#include "config.h"	// this file contains your personal settings

#define serialRX 5    // 5 is D1 on Wemos D1 mini V2
#define serialCTS 12  // 12 is D6 on Wemos D1 mini V2

WiFiClientSecure client;
MeterP1 p1;

void setup () {
  Serial.begin(115200);
  p1.begin(serialCTS, serialRX);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  setupOTA();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  // set sha fingerprint for ssl connection
  client.setFingerprint(fingerprint);
}

void setupOTA() {
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname(hostName);

  // No authentication by default
  ArduinoOTA.setPassword(otaPassword);

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
}

void httpRequest() {
  // if there's a successful connection:
  if (client.connect(host, 443)) {
    client.println(p1.sendline);
    Serial.println(p1.sendline);

    client.println("Host: emoncms.tuxic.nl");
    client.println("User-Agent: arduino-ethernet");
    client.println("Connection: close");
    client.println();
    Serial.println("Request sent");
    Serial.println("closing connection");
  }
  else {
    Serial.println("connection failed");
    client.stop();
  }
}

long clocktime;

void loop() {
  p1.readTelegram();    // read a telegram from the meter
  int minutes = p1.getMinutes();
  unsigned long next = millis() + (60 - p1.getSeconds()) * 1000;
  if ((p1.getMinutes() % 15) == 0) {
    p1.printTelegram(node, apikey);
  } else {
    p1.printShortTelegram(node, apikey);
  }
  httpRequest();
  while (millis() < next) {
    delay(1);
    ArduinoOTA.handle();
  }
}
