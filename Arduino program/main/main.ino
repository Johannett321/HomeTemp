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
#define DHTPIN 7        // DHT data pin
#define DHTTYPE DHT11     // DHT 11
DHT dht = DHT(DHTPIN, DHTTYPE); // Starter DHT sensoren
/*----------------------------------------------------------*/

/*----SD-----*/
int pinSD = 5;
/*-----------*/

/*----RTC----*/
RTC_DS1307 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
/*-----------*/

//Variabler
float temp;
int hum;
String tempC;
int refreshFreq = 1000*60*10; //Dersom den skal oppdateres hvert 10. minutt
//int refreshFreq = 1000*60; //Dersom den skal oppdateres hvert minutt
//int refreshFreq = 1000*15; //Dersom den skal oppdateres hvert 15. sekund (brukes til testing)

//SERVER AND CLIENT
WiFiServer server(80);
WiFiClient client;

//WIFI INFO
String ssid;        // Nettverks SSID (navn)
String pass;        // Nettverks passord

boolean forceConfig = false; //Tvinger konfigurering (for eksempel dersom RTC ikke lenger vet hva klokken er).

void setup() {
  Serial.begin(9600); //Dette er bare et notat til Johan: BaudRate_115200.
  delay(2000);
  
  printStartupText(); //Den teksten som sier '---------------DEVICE STARTUP----------------'

  //Her gjøres de forskjellige modulene klare.
  prepareSD();
  prepareRTC();
  prepareDHTTempSensor();

  //Sjekker om enheten har blitt konfigurert før (koblet til wifi og gitt navn osv).
  //SD.remove("config.txt"); //RESET CONFIGURATION
  String configuration = readFromSD("config.txt");
  if (configuration.length() < 1 || forceConfig) {
    forceConfig = false;
    runFirstTimeConfig(); //Kjør førstegangskonfigurasjon
  }else {
    configure(configuration);
  }
  
  checkWifi();
}

//Starter RTC modulen
void prepareRTC() {
  //Dersom den ikke finner RTC modulen
  if (!rtc.begin()) {
   Serial.println("Couldn't find RTC");
   while (1);
  }

  //Dersom RTC modulen ikke kjørte fra før (batteriet er blitt tatt ut), still klokken.
  if (!rtc.isrunning()) {
    Serial.println("RTC lost power");
    forceConfig = true;
  }
}

// Starter temperatursensoren.
void prepareDHTTempSensor() {
  dht.begin();
}

/*
 * LOOP funksjonen:
 * Denne funksjonen kjøres om og om igjen. Mange ganger i millisekundet. 
 * Dette er på en måte roten til all den andre koden. Alt kjøres ut i fra denne.
 * 
 * Den har følgende ansvarsoppgaver:
 * Den henter temperaturdata og lagrer dette så ofte man stiller inn at den skal.
 * Den sjekker om HomeTemp er tilkoblet wifi hvert 6. sekund, og kobler til Wifi dersom man ikke er det. 
 * Den sjekker om det er noen nye enheter tilkoblet, og lytter til kommandoer fra de.
 */
 
void loop(){
  //Henter temperatur hvert 10. minutt, og lagrer dette.
  if (millis() % refreshFreq == 1) {
    getTempAndSave();
  }

  //Sjekker om HomeTemp fortsatt er tilkoblet wifi hvert 60. sekund
  if (millis() % 60000 == 1) {
    checkWifi();
  }

  //Godtar nye telefoner som kobles til HomeTemp, og lytter til deres kommandoer.
  checkNewClients(false);
}

//Henter temperatur og luftfuktighetsdata, og lagrer dette på SD kortet.
void getTempAndSave() {
  readLiveTemp();
  String tempToSave = formatTempWithTime();
  Serial.print("Temp was updated: ");
  Serial.println(tempToSave);
  saveToSD("temps.txt", tempToSave);
}

//Henter temperatur og luftfuktighetsdata fra temperatursensor
void readLiveTemp() {
  a:
  temp = dht.readTemperature();     //Temperatur
  hum = dht.readHumidity();         //Luftfuktighet

  //Dersom temperatursensoren returnerer NaN (Not a Number). Altså at den feiler med å lese
  if (isnan(hum) || isnan(temp)) {
    Serial.println("Failed to read sensor");
    delay(1000);
    goto a;
  }
}

//Lager en string med nåværende millisekunder siden 1970 og legger til temperatur og luftfuktighet med ':' som skilletegn.
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
