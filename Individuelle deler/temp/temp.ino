/*   Arduino IOT - Temperature (oC) and Humidity (%) on the web
  *Use the DHT-22 sensor to read temperature and humidity values
  *Send these values to www.thingSpeak.com with the ESP8266 serial Wifi module
  Dev: Michalis Vasilakis // Date:23/2/2016 // Update: 25/2/2015 // Ver. 1.3
  More info: http://www.ardumotive.com/iot-wifi-temp-and-humidity.html
  Tip: open the serial monitor for debugging                    */

//Libraires
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <SD.h>
#include <SPI.h>

/*------------------------DHT SENSOR------------------------*/
#define DHTPIN 7        // DHT data pin connected to Arduino pin 7
#define DHTTYPE DHT11     // DHT 22 (or AM2302)
DHT dht = DHT(DHTPIN, DHTTYPE); // Initialize the DHT sensor
/*----------------------------------------------------------*/

File myFile;

//Variables
float temp;
int hum;
String tempC;

int pinSD = 10;

void setup() {
  Serial.begin(9600); //or use default 115200.
  while(!Serial) {}
  
  Serial.println("Preparing SD card...");
  pinMode(pinSD, OUTPUT);

  // SD Card Initialization
  if (SD.begin()) {
    Serial.println("SD card is ready to use.");
    dht.begin();
  }else {
    Serial.println("SD card initialization failed");
    abort();
  }
}

void loop(){
  //Read temperature and humidity values from DHT sensor:
  temp = dht.readTemperature();
  hum = dht.readHumidity();

  if (isnan(hum) || isnan(temp)) {
    Serial.println("Failed to read sensor");
    delay(1000);
    return;
  }

  Serial.print("Current temp: ");
  Serial.println(temp);

  String lastTempFile = readFromSD("temps.txt");
  lastTempFile += temp;
  saveToSD("temps.txt", lastTempFile);

  Serial.print("Read temp: ");
  Serial.println(lastTempFile);
  
  delay(3000); //Update every 1 hour
}

void saveToSD(String filename, String content) {
  // Create/Open file
  myFile = SD.open(filename, FILE_WRITE);

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
  myFile = SD.open(filename);
  
  if (myFile) {
    // Reading the whole file

    String readData = "";
    
    while (myFile.available()) {
      readData += myFile.read();
    }
    myFile.close();
    return readData;
  }else {
    Serial.print("error reading from ");
    Serial.println(filename);
  }

  myFile.close();
  return "";
}
