#define BLYNK_TEMPLATE_ID "TMPL3DCEe2tMl"
#define BLYNK_TEMPLATE_NAME "Virtual_Fence_Core"
#define BLYNK_AUTH_TOKEN "ih2AmCX_6q6qBEticZxfPpFKBB9WChFR"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>

char ssid[] = "vivo Y28";
char pass[] = "12345678901";

#define BUZZER_PIN 25

// ===== BLE TARGET =====
std::string targetMAC = "18:93:d7:33:21:1e";

// ===== STATE VARIABLES =====
float rssiFiltered = 0;
int stateCounter = 0;
int currentLevel = 1;
int previousLevel = 1;

unsigned long lastSeenTime = 0;
bool devicePresent = false;

BLEScan* pBLEScan;

// ===== SEND ALERT =====
void sendNotification(int level)
{
  if (level == 2)
  {
    Serial.println("STATUS: APPROACH ALERT");
    Blynk.logEvent("approach_alert");
    tone(BUZZER_PIN, 2000, 300);
  }

  else if (level == 3)
  {
    Serial.println("STATUS: BREACH ALERT");
    Blynk.logEvent("breach_alert");
    tone(BUZZER_PIN, 2000, 500);
  }

  else if (level == 4)
  {
    Serial.println("STATUS: CRITICAL ALERT");
    Blynk.logEvent("critical_alert");
    tone(BUZZER_PIN, 2000, 1000);
  }
}

// ===== BLE CALLBACK =====
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks
{
  void onResult(BLEAdvertisedDevice advertisedDevice)
  {

    if (advertisedDevice.getAddress().toString() == targetMAC)
    {
      devicePresent = true;
      lastSeenTime = millis();

      int rssiValue = advertisedDevice.getRSSI();

      // RSSI smoothing
      rssiFiltered = (rssiFiltered * 0.6) + (rssiValue * 0.4);

      Serial.print("RSSI: ");
      Serial.println(rssiFiltered);

      // DEBUG: show calculated zone
      Serial.print("ZONE LEVEL: ");
    Serial.println(currentLevel);

      int newLevel = 1;

      // ===== ZONE THRESHOLDS =====
      if (rssiFiltered > -30)
        newLevel = 1;
      else if (rssiFiltered > -45)
        newLevel = 2;
      else if (rssiFiltered > -60)
        newLevel = 3;
      else
        newLevel = 4;

      // ===== PERSISTENCE FILTER =====
      if (newLevel == currentLevel)
        stateCounter++;
      else
      {
        stateCounter = 0;
        currentLevel = newLevel;
      }

    if (stateCounter >= 2)
{
  if (currentLevel != previousLevel)
  {
    if (currentLevel > 1)  // do not notify SAFE zone
    {
      sendNotification(currentLevel);
    }

    previousLevel = currentLevel;
  }
}

      Serial.println("----------------------");
    }
  }
};

// ===== SETUP =====
void setup()
{
  Serial.begin(115200);

  pinMode(BUZZER_PIN, OUTPUT);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
}

// ===== LOOP =====
void loop()
{
  Blynk.run();

  BLEScanResults foundDevices = pBLEScan->start(1, false);
  pBLEScan->clearResults();

  // ===== DEVICE LOST DETECTION =====
  if (devicePresent && millis() - lastSeenTime > 6000)
  {
    devicePresent = false;

    Serial.println("WARNING: CHILD NODE LOST");

    Blynk.logEvent("critical_alert");

    tone(BUZZER_PIN, 2000, 1500);
  }
}