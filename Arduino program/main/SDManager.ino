// Starter SD kort leseren
void prepareSD() {
  Serial.println("Preparing SD card...");
  
  if (SD.begin(pinSD)) {
    Serial.println("SD card is ready to use.");
  }else {
    Serial.println("SD card initialization failed");
    abort();
  }
}

//Lagrer tekst i en fil på minnekortet
void saveToSD(String filename, String content) {
  // Lager/åpner filen som skal oppdateres
  Serial.println("Opening file (WRITE): " + filename);
  File myFile = SD.open(filename, FILE_WRITE);

  // Hvis filen er åpen, skriv til den
  if (myFile) {
    Serial.println("Writing to file...");
    myFile.println(content);  //Skriver tekst til minnekortet
    Serial.println("Done writing to file!");
  }else {
    //Feilet med å lagre på minnekortet
    Serial.print("ERROR saving to ");
    Serial.println(filename);
  }
  myFile.close();
}

// Leser fra minnekortet
String readFromSD(String filename) {
  Serial.println("Opening file (READ): " + filename);
  File myFile = SD.open(filename, FILE_READ); //Filen som skal leses
  
  if (myFile) {
    Serial.println("Starting read...");
    String readData = "";               //Stringen som skal holde innholdet som blir lest
    
    while (myFile.available()) {
      readData += (char)myFile.read(); //Så lenge det er mer igjen i filen å lese, så les videre.
    }
    myFile.close();                    //Filen er ferdig lest, lukk den.
    return readData;                   //Returner det som er lest
  }else {
    //Klarte ikke lese fra filen
    Serial.print("ERROR reading from ");
    Serial.println(filename);
    myFile.close();
    return "";
  }
}

// Leser fra minnekortet, men beholder ikke alt som er lagret etter en viss dato, og før en annen.
String readFromSDWithTimeFrame(String filename, unsigned long millisStart, unsigned long millisEnd) {
  Serial.println("Reading from SD with time frame: " + filename);
  
  File myFile = SD.open(filename, FILE_READ); //Filen som skal leses
  
  if (myFile) {
    Serial.println("Starting read...");
    String readData = ""; //Stringen som skal holde innholdet som blir lest

    String temporaryLine = ""; //Hver linje som blir lest fra filen, blir midlertidig lagret i denne
    while (myFile.available()) {
      char readCharacter = (char) myFile.read(); // Leser én bokstav fra filen
      temporaryLine += readCharacter;
      
      if (readCharacter == '\n') {
        //Ble nettop ferdig å lese en hel linje, nå: analyser den.
        String millisOfRead = getValue(temporaryLine, ':', 0);
        long millisOfReadLong = millisOfRead.toInt();
        if (millisOfReadLong > millisStart && millisOfReadLong < millisEnd) {
          readData += temporaryLine; //Dersom dataene ble lagret etter startdato, og før sluttdato, vil dataene bli beholdt.
        }
        temporaryLine = "";
      }
    }
    Serial.println("Done reading...");
    myFile.close();                    //Filen er ferdig lest, lukk den.
    return readData;                   //Returner det som er lest
  }else {
    //Klarte ikke lese fra filen
    Serial.print("ERROR reading from ");
    Serial.println(filename);
    myFile.close();
    return "";
  }
}

void createFile(String filename) {
  Serial.print("Creating file with name: ");
  Serial.println("/hometemp/" + filename);

  if(SD.exists("/hometemp")==0) {
    Serial.println("Creating folder 'Hometemp'");
    SD.mkdir("/hometemp");
  }
  
  File myFile = SD.open("/hometemp/" + filename, FILE_WRITE);

  if(SD.exists("/hometemp/" + filename)==0) {
    if (myFile) {
      Serial.println("File created!");
    }else {
      Serial.println("ERROR: Unable to create file: " + filename);
    }
    myFile.close();
  }

  if (myFile) {
    Serial.println("File created!");
  }
  
}
