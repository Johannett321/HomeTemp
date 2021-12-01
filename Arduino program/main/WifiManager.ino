boolean connectionSuccessLastTime = false;

int failCount = 0;

//Sjekker om wifi er tilkoblet, og eventuelt kobler til dersom det ikke er gjort.
void checkWifi() {

  //Prøver å koble til wifi, til wifi er tilkoblet
  while (status != WL_CONNECTED) {
    connectionSuccessLastTime = false;
    if (status == 4) {
      failCount += 1;
      Serial.println("----------------------------------------");
      Serial.println("Failed to connect!");

      if (failCount == 4) {
        failCount = 0;
        runFirstTimeConfig();
      }
      
    }else if (status == 0) {
      Serial.println("----------------------------------------");
      Serial.println("Connecting to wifi...");
    }

    char ssidCharArray[ssid.length() + 1];
    ssid.toCharArray(ssidCharArray, ssid.length() + 1);

    char passCharArray[pass.length() + 1];
    pass.toCharArray(passCharArray, pass.length() + 1);

    Serial.print("Attempting to connect to network: ");
    Serial.print(String(ssidCharArray));
    Serial.print(" with password: ");
    Serial.println(String(passCharArray));

    status = WiFi.begin(ssidCharArray, passCharArray);
    delay(3000);
  }

  delay(1); //Denne linjen sørger for at koden ikke kjøres mange ganger i millisekundet i LOOP funksjonen.

  //Når wifi er tilkoblet, startes serveren, og printer lokal IP.
  if (!connectionSuccessLastTime) {
    connectionSuccessLastTime = true;
    
    Serial.println("You're connected to the network");
    server.begin();
    IPAddress myAddress = WiFi.localIP();
    Serial.print("IP: ");
    Serial.println(myAddress);
    Serial.println("----------------------------------------");
  }
}

//Sjekker om nye enheter (telefoner) prøver å koble til HomeTemp enheten.
void checkNewClients(boolean configMode) {
  client = server.available();
  
  if (client) {
    //En enhet er tilkoblet, start å les linjen mellom enheten og Arduinoen
    Serial.println("New client!");
    if (client.available()) {
      char c = client.read();     //En character som mottas fra telefonen

      //Når telefonen sier den er ferdig å sende data, leser Arduinoen hele linjen som ble sendt.
      if (c == '\n') {
        if (configMode) {
          // HomeTemp er ikke konfigurert, og får beskjeder om å konfigureres
          decodeConfigCommand();
        }else {
          decodeCommand();
        }
        header = "";
      }else {
        header += c;
      }
    }
  }
}

// Bestemmer hva som skal gjøres ut i fra linjen kode som ble mottatt.
void decodeCommand() {
  Serial.println("Received command: " + header);
  String commandReq = getValue(header, ';', 0);
  String command = getValue(header, ';', 1);
  String parameters = getValue(header, ';', 2);
  
  if (command == "STOP") {
    client.stop();
    Serial.println("Client disconnected");
  }else if (command == "CURRENT_TEMP") {
    readLiveTemp();                                       //Leser nåværende temperatur
    String answer = String(temp) + "," + String(hum);     //Slår sammen temperatur og luftfuktighet med ';'som skilletegn
    sendAnswer(commandReq, answer);                       //Sender temperatur og luftfuktighetsdataen til telefonen
  }else if (command == "DEV_ID") {
    String answer = "9847488382";
    sendAnswer(commandReq, answer); //Sender device ID'en til telefonen
  }else if (command == "HISTORY") {
    //Henter historikk over målinger innen en viss tidsperiode
    String startTimeString = getValue(parameters, ',', 0);
    String endTimeString = getValue(parameters, ',', 1);

    unsigned long startTime = convertToLong(startTimeString);
    unsigned long endTime = convertToLong(endTimeString);
    
    
    String answer = readFromSDWithTimeFrame("temps.txt", startTime, endTime); //Leser alle temperaturmålingene fra SD kortet i en viss time frame, og lager et svar ut av det
    Serial.println("----------REQUEST HISTORY------------");
    Serial.println(answer); //Skriver de leste temperaturmålingene til konsollen.
    Serial.println("-------------------------------------");

    char c = '\n';
    answer.replace('\n', ','); //Fjerner newline alle plasser i svaret til HomeTemp
    answer = removeLineBreaks(answer);
    answer.trim(); //Fjerner newlines her og
    
    sendAnswer(commandReq, answer); //Sender svaret til HomeTemp
  }else if (command == "deviceInfo") {
    //Sender informasjon om enhetens imei og navnet dens.
    String answer = deviceImei;
    answer += ",";
    answer += deviceName;
    sendAnswer(commandReq, answer);
  }else if (command == "deleteConfig") {
    //Sletter konfigurasjonsfilen med informasjon om wifi passord og enhetsnavn osv
    SD.remove("config.txt");
    sendAnswer(commandReq, "SUCCESS");
  }else if (command == "setRefreshFreq") {
    SD.remove("freq.txt");
    saveToSD("freq.txt", parameters);
    refreshFreq = parameters.toInt();
    sendAnswer(commandReq, "SUCCESS");
  }
}

//Skriver selve svaret på forespørselen til telefonen, og sender dette over linjen
void sendAnswer(String reqID, String answer) {
  Serial.println("Answering client...");
  delay(200);
  Serial.println(answer);
  client.print(reqID);
  client.print(";");
  client.print(answer);
  client.print(String('\r'));
  client.print(String('\n'));
  Serial.println("Client answered!");
}

//Bruker en character til å splitte en string
String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}
