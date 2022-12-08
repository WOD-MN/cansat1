#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>

const int chipSelect = 10;
Adafruit_BMP280 bmp; // I2C

static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;


TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);


void setup() {
  Serial.begin(9600);
  ss.begin(GPSBaud);
  bmp.begin();
  SD.begin(chipSelect);
    
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
}

void loop() {

  finll();
  sdd();

  
  smartDelay(1000);
    
}

void displayInfo()
{
   
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("INVALID"));
  }

 
}

void bmpp(){
  
    
    Serial.print(bmp.readTemperature());
    Serial.print(",");

    
    Serial.print(bmp.readPressure());
    Serial.print(",");
    

    
    Serial.print(bmp.readAltitude(1015.24)); /* Adjusted to local forecast! */
    

    
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
void finll(){
  displayInfo();
  Serial.print(",");
  bmpp();
  Serial.println();
  }


void sdd(){

  String dataString = "";
  String dataString2 = "";
  String dataString3 = "";
  String dataString4 = "";
  String dataString5 = "";
  String dataString6 = "";
  String dataString7 = "";

    dataString = String(gps.altitude.meters(), 3);
    dataString2 = String(gps.location.lat(), 6);
    dataString3 = String(gps.location.lng(), 6); 
    dataString4 = String(bmp.readTemperature());
    dataString5 = String(bmp.readPressure());
    dataString6 = String(bmp.readAltitude(1015.24));
    dataString7 = String(millis());
  
  
  File dataFile = SD.open("GPS.txt", FILE_WRITE);
    dataFile.print(dataString7);
    dataFile.print(",");
   // dataFile.print("GPS Altitude: ");
    dataFile.print(dataString);
    dataFile.print(",");
   // dataFile.print("Latitude: ");
    dataFile.print(dataString2);
    dataFile.print(",");
   // dataFile.print("Longitude: ");
    dataFile.print(dataString3);
    dataFile.print(",");
   // dataFile.print("Temperature: ");
    dataFile.print(dataString4);
    dataFile.print(",");
   // dataFile.print("Atmospheric Pressure: ");
    dataFile.print(dataString5);
    dataFile.print(",");
   // dataFile.print("BMP Altitude: ");
    dataFile.print(dataString6);
    
    dataFile.println();
    dataFile.close();
  
  
  }
