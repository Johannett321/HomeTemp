/*   Arduino IOT - Temperature (oC) and Humidity (%) on the web
  *Use the DHT-22 sensor to read temperature and humidity values
  *Send these values to www.thingSpeak.com with the ESP8266 serial Wifi module
  Dev: Michalis Vasilakis // Date:23/2/2016 // Update: 25/2/2015 // Ver. 1.3
  More info: http://www.ardumotive.com/iot-wifi-temp-and-humidity.html
  Tip: open the serial monitor for debugging                    */

//RTC module
#include "RTClib.h"

//DHT sensor
#include <Adafruit_Sensor.h>
#include <DHT.h>

//SD card reader
#include <SD.h>
#include <SPI.h>

//Wifi sensor
#include <WiFiNINA.h>

/*------------------------DHT SENSOR------------------------*/
#define DHTPIN 7        // DHT data pin connected to Arduino pin 7
#define DHTTYPE DHT11     // DHT 22 (or AM2302)
DHT dht = DHT(DHTPIN, DHTTYPE); // Initialize the DHT sensor
/*----------------------------------------------------------*/

/*----SD-----*/
int pinSD = 5;
/*-----------*/

/*----RTC----*/
RTC_DS1307 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
/*-----------*/

//Variables
float temp;
int hum;
String tempC;
int refreshFreq = 1000*60*10;
//int refreshFreq = 1000*15;

//SERVER AND CLIENT
WiFiServer server(80);
WiFiClient client;

void setup() {
  Serial.begin(9600); //or use default 115200.
  while(!Serial) {}

  prepareRTC();

  prepareSD();
  prepareDHTTempSensor();
  checkWifi();
}

void prepareSD() {
  // SD Card Initialization
  Serial.println("Preparing SD card...");
  pinMode(pinSD, OUTPUT);
  
  if (SD.begin()) {
    Serial.println("SD card is ready to use.");
    prepareDHTTempSensor();
  }else {
    Serial.println("SD card initialization failed");
    abort();
  }
  //SD.remove("temps.txt"); //Sletter temperaturfilen
}

void prepareDHTTempSensor() {
  dht.begin();
}

void prepareRTC() {
  if (!rtc.begin()) {
   Serial.println("Couldn't find RTC");
   while (1);
 }

 if (! rtc.isrunning()) {
   Serial.println("RTC lost power");
   // following line sets the RTC to the date & time this sketch was compiled
   rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
   // This line sets the RTC with an explicit date & time, for example to set
   // January 21, 2014 at 3am you would call:
   // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
 }
}



/*--------LOOP---------*/



void loop(){
  if (millis() % refreshFreq == 1) {
    getTempAndSave();
  }

  //Check if still connected to wifi
  if (millis() % 60000 == 1) {
    checkWifi();
  }

  //Accept new incoming clients
  checkNewClients();
}

void getTempAndSave() {
  readLiveTemp();
  //Print and save temp and time
  String tempToSave = formatTempWithTime();
  Serial.print("Temp was updated: ");
  Serial.println(tempToSave);
  saveToSD("temps.txt", tempToSave);
}

void readLiveTemp() {
  //Read temperature and humidity values from DHT sensor:
  temp = dht.readTemperature();
  hum = dht.readHumidity();

  if (isnan(hum) || isnan(temp)) {
    Serial.println("Failed to read sensor");
    delay(1000);
    return;
  }
}

String formatTempWithTime() {
  DateTime now = rtc.now();

  String tempWithTime = "";
  tempWithTime += now.unixtime();
  tempWithTime += ":";
  tempWithTime += temp;
  tempWithTime += ":";
  tempWithTime += hum;
  return tempWithTime;
}

void saveToSD(String filename, String content) {
  // Create/Open file
  File myFile = SD.open(filename, FILE_WRITE);

  // If the file opened okay, write to it:
  if (myFile) {
    Serial.println("Writing to file...");
    // Write to file
    myFile.println(content);
    Serial.println("Done writing to file!");
  }else {
    Serial.print("error saving to ");
    Serial.println(filename);
  }
  myFile.close();
}

String readFromSD(String filename) {
  // Reading the file
  File myFile = SD.open(filename);
  
  if (myFile) {
    // Reading the whole file

    String readData = "";
    
    while (myFile.available()) {
      readData += (char)myFile.read();
    }
    myFile.close();
    return readData;
  }else {
    Serial.print("error reading from ");
    Serial.println(filename);
    myFile.close();
    return "";
  }
}
