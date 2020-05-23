#include "MeterP1.h"

MeterP1::MeterP1() {
}

void MeterP1::begin(int enablePin, int serialRX) {
  // turn off P1 port telegram sending at start
  p1Serial.begin(115200, SWSERIAL_8N1, serialRX, -1, true);
  pinMode(enablePin, OUTPUT);
  this->enablePin = enablePin;
  disable();
}

void MeterP1::readTelegram() {
  enable();
  tflush();
  readSingleTelegram();
  disable();
}

void MeterP1::printTelegram(const char node[], const char apikey[]) {
  sprintf(sendline, "GET /input/post?node=%s&apikey=%s&json={T181:%d,T182:%d,T281:%d,T282:%d,P1:%d,P2:%d,IP1:%d,IP2:%d,IP3:%d,NP1:%d,NP2:%d,NP3:%d,GAS:%d} HTTP/1.1",
          node, apikey, mEVLT, mEVHT, mEOLT, mEOHT, mEAV, mEAT, mIP1, mIP2, mIP3, mNP1, mNP2, mNP3, mGAS);
}

void MeterP1::printShortTelegram(const char node[], const char apikey[]) {
  sprintf(sendline, "GET /input/post?node=%s&apikey=%s&json={P1:%d,P2:%d,IP1:%d,IP2:%d,IP3:%d,NP1:%d,NP2:%d,NP3:%d} HTTP/1.1",
          node, apikey, mEAV, mEAT, mIP1, mIP2, mIP3, mNP1, mNP2, mNP3);
}

int MeterP1::getMinutes() {
  return mTIM / 100;
}

int MeterP1::getSeconds() {
  return mTIM % 100;
}

void MeterP1::enable() {
  // enable serial port
  digitalWrite(enablePin, LOW);
}

void MeterP1::disable() {
  // disable serial port
  digitalWrite(enablePin, HIGH);
}

void MeterP1::tflush() {
  // flush serial port to get ready for reading
  // read until end-mark to start reading a full telegram from the start
  telegram[0] = 0;
  while (telegram[0] != '!') {
    int len = p1Serial.readBytesUntil('\n', telegram, MAXP1LINELENGTH);
    telegram[len] = 0;
    yield();
  }
}

void MeterP1::readSingleTelegram() {
  // read one full telegram and store values
  telegram[0] = 0;
  currentCRC=0x0000;
  while (telegram[0] != '!') {
    int len = p1Serial.readBytesUntil('\n', telegram, MAXP1LINELENGTH);
    telegram[len] = 0;
    processTelegramLine(len);
    if (telegram[0] == '!') {
      len = 1;
      
    }
    currentCRC = CRC16(currentCRC, (unsigned char*)telegram, len);
    yield();
  }
  char messageCRC[4];
  strncpy(messageCRC, telegram + 1, 4);
  if (! (strtol(messageCRC, NULL, 16) == currentCRC)) {
    //mEAV = 0; mEAT = 0;
  }
}

void MeterP1::processTelegramLine(int len) {

  // 0-0:1.0.0(200517214933S)    Timestamp
  if (strncmp(telegram, "0-0:1.0.0", strlen("0-0:1.0.0")) == 0)
    mTIM = getTimeValue(telegram, len);

  // 1-0:1.8.1(000992.992*kWh)
  // 1-0:1.8.1 = Elektra verbruik laag tarief (DSMR v4.0)
  if (strncmp(telegram, "1-0:1.8.1", strlen("1-0:1.8.1")) == 0)
    mEVLT =  getValue(telegram, len);


  // 1-0:1.8.2(000560.157*kWh)
  // 1-0:1.8.2 = Elektra verbruik hoog tarief (DSMR v4.0)
  if (strncmp(telegram, "1-0:1.8.2", strlen("1-0:1.8.2")) == 0)
    mEVHT = getValue(telegram, len);


  // 1-0:2.8.1(000348.890*kWh)
  // 1-0:2.8.1 = Elektra opbrengst laag tarief (DSMR v4.0)
  if (strncmp(telegram, "1-0:2.8.1", strlen("1-0:2.8.1")) == 0)
    mEOLT = getValue(telegram, len);


  // 1-0:2.8.2(000859.885*kWh)
  // 1-0:2.8.2 = Elektra opbrengst hoog tarief (DSMR v4.0)
  if (strncmp(telegram, "1-0:2.8.2", strlen("1-0:2.8.2")) == 0)
    mEOHT = getValue(telegram, len);


  // 1-0:1.7.0(00.424*kW) Actueel verbruik
  // 1-0:2.7.0(00.000*kW) Actuele teruglevering
  // 1-0:1.7.x = Electricity consumption actual usage (DSMR v4.0)
  if (strncmp(telegram, "1-0:1.7.0", strlen("1-0:1.7.0")) == 0)
    mEAV = getValue(telegram, len);
  if (strncmp(telegram, "1-0:2.7.0", strlen("1-0:2.7.0")) == 0)
    mEAT = getValue(telegram, len);

  // 1-0:21.7.0(00.023*kW)       Instantaneousactive power L1 (+P)
  // 1-0:41.7.0(00.041*kW)       Instantaneousactive power L2 (+P)
  // 1-0:61.7.0(00.347*kW)       Instantaneousactive power L3 (+P)
  // 1-0:22.7.0(00.000*kW)       Instantaneousactive power L1 (-P)
  // 1-0:42.7.0(00.000*kW)       Instantaneousactive power L1 (-P)
  // 1-0:62.7.0(00.000*kW)       Instantaneousactive power L1 (-P)
  if (strncmp(telegram, "1-0:21.7.0", strlen("1-0:21.7.0")) == 0)
    mIP1 = getValue(telegram, len);
  if (strncmp(telegram, "1-0:41.7.0", strlen("1-0:41.7.0")) == 0)
    mIP2 = getValue(telegram, len);
  if (strncmp(telegram, "1-0:61.7.0", strlen("1-0:61.7.0")) == 0)
    mIP3 = getValue(telegram, len);
  if (strncmp(telegram, "1-0:22.7.0", strlen("1-0:22.7.0")) == 0)
    mNP1 = getValue(telegram, len);
  if (strncmp(telegram, "1-0:42.7.0", strlen("1-0:42.7.0")) == 0)
    mNP2 = getValue(telegram, len);
  if (strncmp(telegram, "1-0:62.7.0", strlen("1-0:62.7.0")) == 0)
    mNP3 = getValue(telegram, len);

  // 0-1:24.2.1(150531200000S)(00811.923*m3)
  // 0-1:24.2.1 = Gas (DSMR v4.0) on Kaifa MA105 meter
  if (strncmp(telegram, "0-1:24.2.1", strlen("0-1:24.2.1")) == 0)
    mGAS = getValue(telegram, len);

}

bool MeterP1::isNumber(char* res, int len) {
  for (int i = 0; i < len; i++) {
    if (((res[i] < '0') || (res[i] > '9'))  && (res[i] != '.' && res[i] != 0)) {
      return false;
    }
  }
  return true;
}

int MeterP1::FindCharInArrayRev(char array[], char c, int len) {
  for (int i = len - 1; i >= 0; i--) {
    if (array[i] == c) {
      return i;
    }
  }
  return -1;
}

long MeterP1::getValue(char* buffer, int maxlen) {
  int s = FindCharInArrayRev(buffer, '(', maxlen - 2);
  if (s < 8) return 0;
  if (s > 32) s = 32;
  
  int l = FindCharInArrayRev(buffer, '*', maxlen - 2) - s - 1;
  if (l < 4) return 0;
  if (l > 12) return 0;
  char res[16];
  memset(res, 0, sizeof(res));

  if (strncpy(res, buffer + s + 1, l)) {
    if (isNumber(res, l)) {
      return (1000 * atof(res));
    }
  }
  return 0;
}

int MeterP1::getTimeValue(char* buffer, int maxlen) {
  char res[5];
  strncpy(res, buffer + 18, 4);
  return (atof(res));
}

unsigned int MeterP1::CRC16(unsigned int crc, unsigned char *buf, int len)
{
  for (int pos = 0; pos < len; pos++)
  {
    crc ^= (unsigned int)buf[pos];    // XOR byte into least sig. byte of crc

    for (int i = 8; i != 0; i--) {    // Loop over each bit
      if ((crc & 0x0001) != 0) {      // If the LSB is set
        crc >>= 1;                    // Shift right and XOR 0xA001
        crc ^= 0xA001;
      }
      else                            // Else LSB is not set
        crc >>= 1;                    // Just shift right
    }
  }

  return crc;
}
