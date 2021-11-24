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
int refreshFreq = 1000*60*10; //Dersom den skal oppdateres hvert 10. minutt
//int refreshFreq = 1000*15; //Dersom den skal oppdateres hvert 15. sekund (brukes til testing)

//SERVER AND CLIENT
WiFiServer server(80);
WiFiClient client;

void setup() {
  Serial.begin(9600); //Dette er bare et notat til Johan: BaudRate_115200.
  while(!Serial) {}

  //Her gjøres de forskjellige modulene klare.
  prepareRTC();
  prepareSD();
  prepareDHTTempSensor();
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
  if (! rtc.isrunning()) {
    Serial.println("RTC lost power");
   
    // Stiller klokken på RTC modulen til når filen ble kompilert
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

// Starter SD kort leseren
void prepareSD() {
  Serial.println("Preparing SD card...");
  pinMode(pinSD, OUTPUT);
  
  if (SD.begin()) {
    Serial.println("SD card is ready to use.");
  }else {
    Serial.println("SD card initialization failed");
    abort();
  }
  //SD.remove("temps.txt"); //Sletter temperaturfilen dersom dette ønskes (brukes til testing)
}

// Starter temperatursensoren.
void prepareDHTTempSensor() {
  dht.begin();
}



/*--------LOOP---------*/



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

  //Sjekker om HomeTemp fortsatt er tilkoblet wifi hvert 6. sekund
  if (millis() % 60000 == 1) {
    checkWifi();
  }

  //Godtar nye telefoner som kobles til HomeTemp, og lytter til deres kommandoer.
  checkNewClients();
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
  temp = dht.readTemperature();     //Temperatur
  hum = dht.readHumidity();         //Luftfuktighet

  //Dersom temperatursensoren returnerer NaN (Not a Number). Altså at den feiler med å lese
  if (isnan(hum) || isnan(temp)) {
    Serial.println("Failed to read sensor");
    delay(1000);
    return;
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

//Lagrer tekst i en fil på minnekortet
void saveToSD(String filename, String content) {
  // Lager/åpner filen som skal oppdateres
  File myFile = SD.open(filename, FILE_WRITE);

  // Hvis filen er åpen, skriv til den
  if (myFile) {
    Serial.println("Writing to file...");
    myFile.println(content);  //Skriver tekst til minnekortet
    Serial.println("Done writing to file!");
  }else {
    //Feilet med å lagre på minnekortet
    Serial.print("error saving to ");
    Serial.println(filename);
  }
  myFile.close();
}

// Leser fra minnekortet
String readFromSD(String filename) {
  File myFile = SD.open(filename); //Filen som skal leses
  
  if (myFile) {
    String readData = "";               //Stringen som skal holde innholdet som blir lest
    
    while (myFile.available()) {
      readData += (char)myFile.read(); //Så lenge det er mer igjen i filen å lese, så les videre.
    }
    myFile.close();                    //Filen er ferdig lest, lukk den.
    return readData;                   //Returner det som er lest
  }else {
    //Klarte ikke lese fra filen
    Serial.print("error reading from ");
    Serial.println(filename);
    myFile.close();
    return "";
  }
}
