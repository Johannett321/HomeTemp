//WIFI INFO
char ssid[] = "JohanH02";        // your network SSID (name)
char pass[] = "8438936427";        // your network password (use for WPA, or use as key for WEP)
int status = WL_IDLE_STATUS;     // the Wifi radio's status

String header = "";

boolean connectionSuccessLastTime = false;

void checkWifi() {
  // attempt to connect to Wifi network:
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
    delay(6000);
  }

  if (!connectionSuccessLastTime) {
    connectionSuccessLastTime = true;
    
    // you're connected now, so print out the data:
    Serial.println("You're connected to the network");
    server.begin();
    IPAddress myAddress = WiFi.localIP();
    Serial.print("IP: ");
    Serial.println(myAddress);
    Serial.println("----------------------------------------");
  }
}

void checkNewClients() {
  client = server.available();
  if (millis() % 5000 == 1) {
    if (client.connected()) {
      //Serial.println("Connected client :/");
    }else {
      //Serial.println("NOT CONNECTED!");
    }
      
  }
  
  if (client) {
    Serial.println("New client!");
    if (client.available()) {
      char c = client.read();

      if (c == '\n') {
        decodeCommand();
        header = "";
      }else {
        header += c;
      }
    }
  }
}

void decodeCommand() {
  Serial.println("Received command: " + header);
  String commandReq = getValue(header, ';', 0);
  String command = getValue(header, ';', 1);
  String parameters = getValue(header, ';', 2);
   
  if (command == "STOP") {
    client.stop();
    Serial.println("Client disconnected");
  }else if (command == "CURRENT_TEMP") {
    readLiveTemp();
    String answer = String(temp) + "," + String(hum);
    sendAnswer(commandReq, answer);
  }else if (command == "DEV_ID") {
    client.println("42909102");
  }
}

void sendAnswer(String reqID, String answer) {
  Serial.println("Answering client...");
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
