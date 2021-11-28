char ssidSetup[] = "HomeTemp Device";        // SSID til HomeTemp i konfigureringsmodus
char passSetup[] = "00000000";               // Passordet til HomeTemp i konfigureringsmodus
int status = WL_IDLE_STATUS;                 // Wifiens radio status

String header = "";

boolean configurationFinished = false; //Sørger for at HomeTemp holdes i konfigurasjonsmodus til den er konfigurert

String deviceName = "";
String deviceImei = "";

//Dersom Arduino ikke klarer å koble til Wifi, eller ikke vet Wifi info, kjøres dette.
void runFirstTimeConfig() {
  // print the network name (SSID);
  Serial.println("Device not configured, or configuration is wrong. Entering configuration mode...");
  Serial.println("--------------------------------------");
  Serial.print("Creating access point named: ");
  Serial.println(ssidSetup);

  status = WiFi.beginAP(ssidSetup, passSetup); // Lager et Wifi accesspoint som man kan koble til for å konfigurere.
  if (status != WL_AP_LISTENING) {
    Serial.println("Creating access point failed");
    // don't continue
    while (true);
  }

  // wait 10 seconds for connection:
  delay(10000);

  // start the web server on port 80
  server.begin(); // Starter en server på port 80 som mobilen kan kommunisere med. 

  IPAddress myAddress = WiFi.localIP();
  Serial.print("IP: ");
  Serial.println(myAddress); // Printer HomeTemp sin IP
  Serial.println("----------------------------------------");

  while (!configurationFinished) {
    checkNewClients(true); // Sjekker om nye mobiler er tilkoblet
  }
  configurationFinished = false;
}


//Leser kommandoer fra mobiler i config modus.
void decodeConfigCommand() {
  Serial.println("Received command: " + header);
  String commandReq = getValue(header, ';', 0);
  String command = getValue(header, ';', 1);
  String parameters = getValue(header, ';', 2);

  if (command == "configure") {
    Serial.println("Configuration: " + parameters);

    //Stiller HomeTemp klokken etter mobilen
    String beforeConvert = getValue(parameters, ',', 4);
    unsigned long millisReceived = convertToLong(beforeConvert);
    Serial.print("BeforeConvert: ");
    Serial.print(beforeConvert);
    Serial.print(". Time to set: ");
    Serial.println(String(millisReceived));
    rtc.adjust(DateTime(millisReceived));

    char c = '\n';
    char r = '\r';
    parameters.replace(String(c), ""); //Fjerner newline alle plasser i svaret til HomeTemp
    parameters.replace(String(r), "");  //Fjerner \r fra alle plasser i svaret til HomeTemp
    SD.remove("config.txt");
    saveToSD("config.txt", parameters);

    //Reset nettverksstatus
    status = WL_IDLE_STATUS;

    configure(parameters); //Konfigurerer enheten
    sendAnswer(commandReq, "SUCCESS;Device configured!"); // Gir beskjed til telefonen om at enheten er konfigurert
    delay(1000);
    configurationFinished = true;
  }else if (command == "deviceInfo") {
    sendAnswer(commandReq, "null");
  }
}

//Konfigurerer HomeTemp etter siste informasjon fra mobil
void configure(String configuration) {
  ssid = getValue(configuration, ',', 0);         //SSID
  pass = getValue(configuration, ',', 1);         //PASS
  deviceName = getValue(configuration, ',', 2);   //Enhetens navn
  deviceImei = getValue(configuration, ',', 3);   //Imei nummer
}
