#define BLYNK_TEMPLATE_ID "TMPL3DCEe2tMl"
#define BLYNK_TEMPLATE_NAME "Virtual Fence Core"
#define BLYNK_AUTH_TOKEN "ih2AmCX_6q6qBEticZxfPpFKBB9WChFR"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>

char ssid[] = "vivo Y28";
char pass[] = "12345678901";

// ====== RSSI + STATE VARIABLES ======

int rssiValue = 0;
float rssiFiltered = 0;

int stateCounter = 0;
int currentLevel = 1;
int previousLevel = 1;

#define SAFE_THRESHOLD      -55
#define APPROACH_THRESHOLD  -65
#define BREACH_THRESHOLD    -75

BLEScan* pBLEScan;
int scanTime = 1;

// ====== SEND NOTIFICATION FUNCTION ======

void sendNotification(int level)
{
  if (level == 2)
  {
    Blynk.logEvent("approach_alert");
  }
  else if (level == 3)
  {
    Blynk.logEvent("breach_alert");
  }
  else if (level == 4)
  {
    Blynk.logEvent("critical_alert");
  }
}

// ====== BLE CALLBACK ======

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks
{
  void onResult(BLEAdvertisedDevice advertisedDevice)
  {
    BLEUUID targetUUID("00001234-0000-1000-8000-00805f9b34fb");

    if (advertisedDevice.haveServiceUUID() &&
        advertisedDevice.getServiceUUID().equals(targetUUID))
    {
      rssiValue = advertisedDevice.getRSSI();
      rssiFiltered = (rssiFiltered * 0.7) + (rssiValue * 0.3);

      int newLevel;

      if (rssiFiltered > SAFE_THRESHOLD)
        newLevel = 1;
      else if (rssiFiltered > APPROACH_THRESHOLD)
        newLevel = 2;
      else if (rssiFiltered > BREACH_THRESHOLD)
        newLevel = 3;
      else
        newLevel = 4;

      if (newLevel == currentLevel)
      {
        stateCounter++;
      }
      else
      {
        stateCounter = 0;
        currentLevel = newLevel;
      }

      if (stateCounter >= 3)
      {
        // Trigger only if escalation
        if (currentLevel > previousLevel)
        {
          sendNotification(currentLevel);
          previousLevel = currentLevel;
        }
      }
    }
  }
};

// ====== SETUP ======

void setup()
{
  Serial.begin(115200);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
}

// ====== LOOP ======

void loop()
{
  Blynk.run();

  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  pBLEScan->clearResults();  // Free memory
}