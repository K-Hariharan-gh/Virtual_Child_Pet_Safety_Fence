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

// ===== BUZZER PIN =====
#define BUZZER_PIN 25

// ===== MAC FILTER =====
std::string targetMAC = "18:93:d7:33:21:1e";

//Noise management 
#define RSSI_WINDOW 5

int rssiSamples[RSSI_WINDOW];
int rssiIndex = 0;
bool windowFilled = false;

unsigned long lastSeenTime = 0;
bool beaconLostAlertSent = false;

#define BEACON_TIMEOUT 10000  // 10 seconds

// ===== STATE VARIABLES =====
float rssiFiltered = 0;
int stateCounter = 0;
int currentLevel = 1;
int previousLevel = 1;

BLEScan* pBLEScan;

bool beaconPreviouslyLost = false;
//---------Global variable End here--------//

// ===== BUZZER FUNCTION =====
void triggerBuzzer(int duration)
{
  digitalWrite(BUZZER_PIN, HIGH);
  delay(duration);
  digitalWrite(BUZZER_PIN, LOW);
}

// ===== SEND EVENT =====
void sendNotification(int level)
{
  if (level == 2)
  {
    Blynk.logEvent("approach_alert");
    triggerBuzzer(200);
  }
  else if (level == 3)
  {
    Blynk.logEvent("breach_alert");
    triggerBuzzer(400);
  }
  else if (level == 4)
  {
    Blynk.logEvent("critical_alert");
    triggerBuzzer(700);
  }
}

// RSSI Averaging function
float getAverageRSSI(int newRSSI)
{
  rssiSamples[rssiIndex] = newRSSI;
  rssiIndex++;

  if (rssiIndex >= RSSI_WINDOW)
  {
    rssiIndex = 0;
    windowFilled = true;
  }

  int count = windowFilled ? RSSI_WINDOW : rssiIndex;

  float sum = 0;
  for (int i = 0; i < count; i++)
  {
    sum += rssiSamples[i];
  }

  return sum / count;
}

// ===== BLE CALLBACK =====
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks
{
  void onResult(BLEAdvertisedDevice advertisedDevice)
  {
    if (advertisedDevice.getAddress().toString() == targetMAC)
{
  lastSeenTime = millis();
  beaconLostAlertSent = false;

  if (beaconPreviouslyLost)
  {
    Serial.println("Beacon reconnected — resetting zone state");

    currentLevel = 1;
    previousLevel = 1;
    stateCounter = 0;

    beaconPreviouslyLost = false;
  }
      int rssiValue = advertisedDevice.getRSSI();

      // RSSI smoothing
      rssiFiltered = getAverageRSSI(rssiValue);

      Serial.print("RSSI: ");
      Serial.println(rssiFiltered);

      int newLevel = 1;

      if (rssiFiltered > -30)
        newLevel = 1;
      else if (rssiFiltered > -45)
        newLevel = 2;
      else if (rssiFiltered > -60)
        newLevel = 3;
      else
        newLevel = 4;

      // Persistence logic
      if (newLevel == currentLevel)
        stateCounter++;
      else
      {
        stateCounter = 0;
        currentLevel = newLevel;
      }

    if (stateCounter >= 2)
{
    // Escalation: SAFE → APPROACH → BREACH → CRITICAL
    if (currentLevel > previousLevel)
    {
        sendNotification(currentLevel);
        previousLevel = currentLevel;
    }

    // Reset escalation if child returns to SAFE zone
    if (currentLevel == 1)
    {
        previousLevel = 1;
    }
}

        // RESET FIX: allow alerts again when returning to SAFE
        if (currentLevel == 1)
        {
          previousLevel = 1;
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
  digitalWrite(BUZZER_PIN, LOW);

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

  // ===== BEACON WATCHDOG =====
 if (millis() - lastSeenTime > BEACON_TIMEOUT && !beaconLostAlertSent)
{
  Serial.println("Beacon signal lost!");

  Blynk.logEvent("critical_alert", "Beacon lost or battery dead");

  triggerBuzzer(1000);

  beaconLostAlertSent = true;
  beaconPreviouslyLost = true;
}
}
