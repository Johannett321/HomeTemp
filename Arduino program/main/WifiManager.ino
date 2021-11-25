//WIFI INFO
char ssid[] = "Johans iPhone";        // Nettverks SSID (navn)
//char pass[] = "8438936427";         // Nettverks passord hjemme
char pass[] = "12345678";             // Nettverks passord iPhone
int status = WL_IDLE_STATUS;          // Wifiens radio status

String header = "";

boolean connectionSuccessLastTime = false;

//Sjekker om wifi er tilkoblet, og eventuelt kobler til dersom det ikke er gjort.
void checkWifi() {

  //Prøver å koble til wifi, til wifi er tilkoblet
  while (status != WL_CONNECTED) {
    connectionSuccessLastTime = false;
    if (status == 4) {
      Serial.println("----------------------------------------");
      Serial.println("Failed to connect!");
    }else if (status == 0) {
      Serial.println("----------------------------------------");
      Serial.println("Connecting to wifi...");
    }
    
    Serial.print("Attempting to connect to network: ");
    Serial.println(ssid);

    status = WiFi.begin(ssid, pass);
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
void checkNewClients() {
  client = server.available();
  
  if (client) {
    //En enhet er tilkoblet, start å les linjen mellom enheten og Arduinoen
    Serial.println("New client!");
    if (client.available()) {
      char c = client.read();     //En character som mottas fra telefonen

      //Når telefonen sier den er ferdig å sende data, leser Arduinoen hele linjen som ble sendt.
      if (c == '\n') {
        decodeCommand();
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
    String answer = readFromSDWithTimeFrame("temps.txt", 1637684371, getRealTimeMillis()); //Leser alle temperaturmålingene fra SD kortet i en viss time frame, og lager et svar ut av det
    Serial.println("----------REQUEST HISTORY------------");
    Serial.println(answer); //Skriver de leste temperaturmålingene til konsollen.
    Serial.println("-------------------------------------");
    
    char c = '\n';
    char r = '\r';
    answer.replace(String(c), ","); //Fjerner newline alle plasser i svaret til HomeTemp
    answer.replace(String(r), "");  //Fjerner \r fra alle plasser i svaret til HomeTemp
    sendAnswer(commandReq, answer); //Sender svaret til HomeTemp
  }
}

//Skriver selve svaret på forespørselen til telefonen, og sender dette over linjen
void sendAnswer(String reqID, String answer) {
  Serial.println("Answering client...");
  Serial.println(answer);
  client.print(reqID);
  client.print(";");
  client.println(answer);
  Serial.println("Client answered!");
}

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
