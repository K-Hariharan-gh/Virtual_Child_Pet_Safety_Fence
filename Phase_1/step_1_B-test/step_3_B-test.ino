#define BLYNK_TEMPLATE_ID "TMPL3glY4lIPH"
#define BLYNK_TEMPLATE_NAME "Virtual Fence""
#define BLYNK_AUTH_TOKEN "7Q9Lkhts-R3kQeAzgdAPyUn2LrTNqA1c"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

char ssid[] = "vivo Y28";
char pass[] = "12345678901";

void setup()
{
  Serial.begin(115200);
  Serial.println("Connecting to WiFi and Blynk...");

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  Serial.println("Connected!");

  // Send test notification
  Blynk.logEvent("virtual_fence", "ESP32 Blynk Test Successful");
}

void loop()
{
  Blynk.run();
}