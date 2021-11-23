#include <SD.h>
#include <SPI.h>

File myFile;

int pinCS = 5;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Starting SD reader");
  pinMode(pinCS, OUTPUT);

  // SD Card Initialization
  if (SD.begin())
  {
    Serial.println("SD card is ready to use.");
  } else
  {
    Serial.println("SD card initialization failed");
    return; 
  }

  // Create/Open file
  myFile = SD.open("test.txt", FILE_WRITE);

  // If the file opened okay, write to it:
  if (myFile) {
    Serial.println("Writing to file...");
    // Write to file
    myFile.println("Testing text 1, 2, 3...");
    myFile.close(); // close the file
    Serial.println("Done.");
  }
  // If the file didn't open, print an error:
  else {
    Serial.println("error opening test.txt");
  }

   // Reading the file
   myFile = SD.open("test.txt");
   if (myFile) {
    Serial.println("Read:");
    // Reading the whole file
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
      myFile.close();
    }
    else {
      Serial.println("error opening test.txt");
   }
   SD.remove("text.txt");
}

void loop() {
  // Empty
}
