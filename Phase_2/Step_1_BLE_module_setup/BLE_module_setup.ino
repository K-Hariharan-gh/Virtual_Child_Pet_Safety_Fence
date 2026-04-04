#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>

BLEScan* pBLEScan;

std::string targetMAC = "18:93:d7:33:21:1e";

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {

    if (advertisedDevice.getAddress().toString() == targetMAC) {

      Serial.print("HM-10 FOUND | RSSI: ");
      Serial.println(advertisedDevice.getRSSI());
      Serial.println("----------------------");
    }
  }
};

void setup() {
  Serial.begin(115200);

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
}

void loop() {
  BLEScanResults foundDevices = pBLEScan->start(3, false);
  pBLEScan->clearResults();
}