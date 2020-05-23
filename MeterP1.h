/*
   MeterP1.h - Library for reading P1 port on energy meters
   Created by Jaap Vermaas, may 2020
   Released into the public domain
*/

#ifndef METERP1_H
#define METERP1_H

#include <Arduino.h>
#define MAXP1LINELENGTH 64 // longest normal line is 47 char (+3 for \r\n\0)
#include <SoftwareSerial.h>

#ifndef SERIALDEBUG
#define SERIALDEBUG false
#endif

class MeterP1 {
  private:
    char telegram[MAXP1LINELENGTH];
    int enablePin = 12;
    // Vars to store meter readings
    long mEVLT = 0; //Meter reading Electrics - consumption low tariff
    long mEVHT = 0; //Meter reading Electrics - consumption high tariff
    long mEOLT = 0; //Meter reading Electrics - return low tariff
    long mEOHT = 0; //Meter reading Electrics - return high tariff
    long mEAV = 0;  //Meter reading Electrics - Actual consumption
    long mEAT = 0;  //Meter reading Electrics - Actual return
    long mGAS = 0;    //Meter reading Gas
    long mIP1 = 0; // Instantaneousactive power L1 (+P)
    long mIP2 = 0; // Instantaneousactive power L2 (+P)
    long mIP3 = 0; // Instantaneousactive power L3 (+P)
    long mNP1 = 0; // Instantaneousactive power L1 (-P)
    long mNP2 = 0; // Instantaneousactive power L2 (-P)
    long mNP3 = 0; // Instantaneousactive power L3 (-P)
    int mTIM = 0; // Timestamp
    unsigned int currentCRC=0;
    
    SoftwareSerial p1Serial;
    void enable();
    void disable();
    void tflush();
    void readSingleTelegram();
    void processTelegramLine(int len);
    bool isNumber(char* res, int len);
    int FindCharInArrayRev(char array[], char c, int len);
    long getValue(char* buffer, int maxlen);
    int getTimeValue(char* buffer, int maxlen);
    unsigned int CRC16(unsigned int crc, unsigned char *buf, int len);
    
  public:
    char sendline[300];
    MeterP1();
    void begin(int enablePin, int serialRX);
    void readTelegram();
    void printTelegram(const char node[], const char apikey[]);
    void printShortTelegram(const char node[], const char apikey[]);
    int getMinutes();
    int getSeconds();
};

#endif
