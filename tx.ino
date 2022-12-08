#include <SPI.h>
#include <LoRa.h>
#include <Adafruit_BMP085.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include "DHT.h"
#include "Wire.h"
#include <MPU6050_light.h>

MPU6050 mpu(Wire);
unsigned long timer = 0;

#define LORA_SS 10
#define LORA_RST 9
#define LORA_DIO0 2


DHT dht = DHT(A1, DHT11,6);

Adafruit_BMP085 bmp;


static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);



void setup() {

  pinMode(LORA_SS, OUTPUT);
  digitalWrite(LORA_SS, HIGH);

  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  Serial.begin(9600);
  Wire.begin();
  ss.begin(GPSBaud);
  dht.begin();
  bmp.begin();
  LoRa.begin(433E6);
  byte status = mpu.begin();
  
  while(status!=0){ } 
  
  delay(1000);
  mpu.calcOffsets();



}

void loop() {

    Serial.println(".......BMP-180........");
    
    Serial.print("Temperature = ");
    Serial.print(bmp.readTemperature());
    Serial.println(" *C");
    
    Serial.print("Pressure = ");
    Serial.print(bmp.readPressure());
    Serial.println(" Pa");

    Serial.print("Altitude = ");
    Serial.print(bmp.readAltitude());
    Serial.println(" meters");
    
    Serial.print("Relative altitude = ");
    Serial.print(int(bmp.readAltitude(bmp.readPressure())));
    Serial.println(" meters");

    Serial.println();


    

    Serial.println(".......DHT-11........");
    
    float humidity = dht.readHumidity(); 
    float temperature = dht.readTemperature();
    
    Serial.print("Humidity = ");
    Serial.print(humidity, 1);
    Serial.println(" %");
    
    Serial.print("Temperature = ");
    Serial.print(temperature, 1);
    Serial.println(" *C");
    Serial.println();


  
  Serial.println(".......mpu........");

    
   mpu.update();
  
  if((millis()-timer)>1000){ // print data every 10ms 
  Serial.print(mpu.getAngleX());
  Serial.println();
  Serial.print(mpu.getAngleY());
  Serial.println();
  Serial.print(mpu.getAngleZ());
  Serial.println();
  timer = millis();  
  }
 

    


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


  Serial.println(".......LIGHT.......");
  
  Serial.print("Light Intensity:");
  Serial.print(int(Light(analogRead(0))));
  Serial.println(" Lux");

  Serial.println();
  

  smartDelay(1000);


   //LORA

LoRa.beginPacket();
  
  LoRa.print("/*");
  
  LoRa.print("SAIMANSAT");
  LoRa.print(",");
  
  LoRa.print(millis());
  LoRa.print(",");
   
  LoRa.print(bmp.readTemperature());
  LoRa.print(",");

  LoRa.print(bmp.readAltitude());
  LoRa.print(",");
  
  LoRa.print((bmp.readPressure()/100));
  LoRa.print(",");
  

  
  LoRa.print(Light(analogRead(0)));
  LoRa.print(",");

  LoRa.print(gps.location.lng(),5);
  LoRa.print(",");

  LoRa.print(gps.location.lat(),5);
  LoRa.print(",");
  
  LoRa.print(gps.altitude.meters());
  LoRa.print(",");

  LoRa.print(mpu.getAngleX());
  LoRa.print(",");

  LoRa.print(mpu.getAngleY());
  LoRa.print(",");

  LoRa.print(mpu.getAngleZ());
  

  
  LoRa.print("*/");
  LoRa.println("");
  
  
  LoRa.endPacket();
    


  


  
    
  
    
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
