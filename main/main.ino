#include "WIfiManager.ino";
#include "TemperatureManager.ino";
#include <SPI.h>;
#include <WiFiNINA.h>;

WiFiServer server(80);

public static int STATUS_LIGHT = 3;
public static String imei = "353103640321332"

void setup() {
  pinMode(STATUS_LIGHT, output);
  while (connectToNetwork() == false);

}

void loop() {
  int temp = readCurrentTemp();
  if (temp != null) {
    saveTempToSD(temp);
  }
}