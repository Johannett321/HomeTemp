char ssid[] = "homenet";          //  your network SSID (name) 
char pass[] = "funker123";   // your network password
int status = WL_IDLE_STATUS;

bool connectToNetwork() {
    digitalWrite(STATUS_LIGHT, HIGH);

    Serial.begin(9600);
    Serial.println("Attempting to connect to WPA network...");
    Serial.print("SSID: ");
    Serial.println(ssid);

    status = WiFi.begin(ssid, pass);
    if ( status != WL_CONNECTED) { 
      Serial.println("Couldn't get a wifi connection");
      return false;
    }else {
       server.begin();
       Serial.print("Connected to wifi. My address:");
       IPAddress myAddress = WiFi.localIP();
       Serial.println(myAddress);
       return true;
    }
}

void decodeCommand(String command) {
    if (command == "getCurrentTemp") {
        String currentTemp = readFromSD("currenttemp.txt");
        client.send(currentTemp);
    }
}
