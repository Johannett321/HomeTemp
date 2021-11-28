//Printer den teksten som vises når Arduino booter opp
void printStartupText() {
  Serial.println("");
  Serial.println("");
  Serial.println("");
  Serial.println("");
  Serial.println("");
  Serial.println("");
  Serial.println("");
  Serial.println("");
  Serial.println("");
  Serial.println("---------------------------------------------");
  Serial.println("---------------DEVICE STARTUP----------------");
  Serial.println("---------------------------------------------");
  Serial.println("");
}

//Returnerer antall millisekunder siden 1970 fra RTC modulen.
long getRealTimeMillis() {
  DateTime now = rtc.now();
  return now.unixtime();
}

//Konverterer en String til en long
long convertToLong(String myString) {
  return strtoul(myString.c_str(), NULL, 10);
}

//Fjerner line breaks fra en string ('\r' og '\n')
String removeLineBreaks(String myString) {
  String modifiedString = "";
  
  for(int i = 0; i < myString.length(); i++)
  {
    char ch = myString.charAt(i);
    if (ch == '\n') {
      modifiedString += ',';
      continue;
    }
    if (ch != '\r') {
      modifiedString += ch;
    }
  }
  
  return modifiedString;
}
