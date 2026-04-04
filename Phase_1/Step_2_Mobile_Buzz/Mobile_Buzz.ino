#define BLYNK_TEMPLATE_ID "TMPL3DCEe2tMl"
#define BLYNK_TEMPLATE_NAME "Virtual Fence Core"
#define BLYNK_AUTH_TOKEN "ih2AmCX_6q6qBEticZxfPpFKBB9WChFR"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>

// -------- WiFi --------
char ssid[] = "vivo Y28";
char pass[] = "12345678901";

// -------- Buzzer --------
#define BUZZER_PIN 25

// -------- RSSI + State --------
int rssiValue = 0;
float rssiFiltered = 0;

int stateCounter = 0;
int currentLevel = 1;
int previousLevel = 1;

#define SAFE_THRESHOLD      -55
#define APPROACH_THRESHOLD  -75
#define BREACH_THRESHOLD    -95

BLEScan* pBLEScan;
int scanTime = 1;  // 1 second scan

// -------- Notification --------
void sendNotification(int level)
{
  if (level == 2)
    Blynk.logEvent("approach_alert");
  else if (level == 3)
    Blynk.logEvent("breach_alert");
  else if (level == 4)
    Blynk.logEvent("critical_alert");
}

// -------- Buzzer Patterns --------
void activateBuzzer(int level)
{
  if (level == 2) {
    tone(BUZZER_PIN, 1000);
    delay(300);
    noTone(BUZZER_PIN);
  }
  else if (level == 3) {
    for(int i=0;i<2;i++){
      tone(BUZZER_PIN, 1200);
      delay(300);
      noTone(BUZZER_PIN);
      delay(200);
    }
  }
  else if (level == 4) {
    for(int i=0;i<4;i++){
      tone(BUZZER_PIN, 1500);
      delay(200);
      noTone(BUZZER_PIN);
      delay(150);
    }
  }
}

// -------- BLE Callback --------
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

      // -------- SERIAL OUTPUT --------
      Serial.print("Live RSSI: ");
      Serial.print(rssiFiltered);
      Serial.print(" dBm | ");

      int newLevel;

      if (rssiFiltered > SAFE_THRESHOLD)
        newLevel = 1;
      else if (rssiFiltered > APPROACH_THRESHOLD)
        newLevel = 2;
      else if (rssiFiltered > BREACH_THRESHOLD)
        newLevel = 3;
      else
        newLevel = 4;

      // Print textual level
      if (newLevel == 1) Serial.println("SAFE");
      else if (newLevel == 2) Serial.println("APPROACHING");
      else if (newLevel == 3) Serial.println("CROSSED");
      else Serial.println("CRITICAL");

      // -------- STATE LOGIC --------
      if (newLevel == currentLevel)
        stateCounter++;
      else {
        stateCounter = 0;
        currentLevel = newLevel;
      }

if (stateCounter >= 3)
{
    if (currentLevel > previousLevel)
    {
        sendNotification(currentLevel);
        activateBuzzer(currentLevel);
        previousLevel = currentLevel;
    }
    else if (currentLevel < previousLevel)
    {
        // Update baseline when child comes closer
        previousLevel = currentLevel;
    }
}

      Serial.println("----------------------------------");
    }
  }
};

// -------- Setup --------
void setup()
{
  Serial.begin(115200);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
}

// -------- Loop --------
void loop()
{
  Blynk.run();

  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  pBLEScan->clearResults();
}