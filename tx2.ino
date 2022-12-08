#include "Wire.h"
#include <SPI.h>
#include <LoRa.h>
#include <MPU6050_light.h>
#include <SoftwareSerial.h>
#include <Adafruit_BMP085.h>
#include <TinyGPS++.h>

#define LORA_SS 10
#define LORA_RST 9
#define LORA_DIO0 2

MPU6050 mpu(Wire);

unsigned long timer = 0;

Adafruit_BMP085 bmp;


static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;

TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);



void setup() {

  pinMode(LORA_SS, OUTPUT);
  digitalWrite(LORA_SS, HIGH);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  
  Serial.begin(9600);
  ss.begin(GPSBaud);
  Wire.begin();
  bmp.begin();
  LoRa.begin(433E6);
  mpu.begin();
  
  Serial.println(F("Calculating offsets, do not move MPU6050"));
  delay(1000);
  mpu.calcOffsets(true,true); // gyro and accelero
  Serial.println("Done!\n");
  
}

void loratx(){
  mpu.update();
  LoRa.beginPacket();
  if(millis() - timer > 500){ // print data every second
    LoRa.print("/*");
    LoRa.print("saiman");
    LoRa.print(",");
    LoRa.print(mpu.getAccX());
    LoRa.print(",");
    LoRa.print(mpu.getAccY());
    LoRa.print(",");
    LoRa.print(mpu.getAccZ());
    LoRa.print(",");
    LoRa.print(mpu.getGyroX());
    LoRa.print(",");
    LoRa.print(mpu.getGyroY());
    LoRa.print(",");
    LoRa.print(mpu.getGyroZ());
    LoRa.print(",");
    LoRa.print((bmp.readPressure())/1000);
    LoRa.print(",");
    LoRa.print(bmp.readTemperature());
    LoRa.print(",");
    LoRa.print(bmp.readAltitude());
    LoRa.print(",");
    LoRa.print(Light(analogRead(0)));
    LoRa.print(",");
    LoRa.print(gps.location.lat(),5);
    LoRa.print(",");
    LoRa.print(gps.location.lng(),5);
    LoRa.print(",");
    LoRa.print(gps.altitude.meters());

    

    
    LoRa.println("*/");
  
    timer = millis();
    LoRa.endPacket();
  }
  
  
  }

void loop() {
  
  loratx();

   Serial.println(".......GPS.......");
  
  

  Serial.print("HDOP: ");
  printFloat(gps.hdop.hdop(), gps.hdop.isValid(), 6, 1);
  Serial.println();

  Serial.print("Latitude: ");
  printFloat(gps.location.lat(), gps.location.isValid(), 11, 6);
  Serial.println();

  Serial.print("Longitude: ");
  printFloat(gps.location.lng(), gps.location.isValid(), 12, 6);
  Serial.println();



  Serial.print("GPS Altitude: ");
  printInt(gps.altitude.meters(), gps.altitude.isValid(), 7);
  Serial.println(" meter");
  
  Serial.println();

  

  smartDelay(100);
  
  

}



static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}

static void printFloat(float val, bool valid, int len, int prec)
{
  if (!valid)
  {
    while (len-- > 1)
      Serial.print('*');
    Serial.print(' ');
  }
  else
  {
    Serial.print(val, prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i=flen; i<len; ++i)
      Serial.print(' ');
  }
  smartDelay(0);
}

static void printInt(unsigned long val, bool valid, int len)
{
  char sz[32] = "*****************";
  if (valid)
    sprintf(sz, "%ld", val);
  sz[len] = 0;
  for (int i=strlen(sz); i<len; ++i)
    sz[i] = ' ';
  if (len > 0) 
    sz[len-1] = ' ';
  Serial.print(sz);
  smartDelay(0);
}

double Light (int RawADC0)
{
  double Vout=RawADC0*0.0048828125;
  int lux=3000/(10*((5-Vout)/Vout));//use this equation if the LDR is in the upper part of the divider
  return lux;
}
