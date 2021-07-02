/*
    Video: https://www.youtube.com/watch?v=oCMOYS71NIU
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleNotify.cpp
    Ported to Arduino ESP32 by Evandro Copercini

   Create a BLE server that, once we receive a connection, will send periodic notifications.
   The service advertises itself as: 6E400001-B5A3-F393-E0A9-E50E24DCCA9E
   Has a characteristic of: 6E400002-B5A3-F393-E0A9-E50E24DCCA9E - used for receiving data with "WRITE" 
   Has a characteristic of: 6E400003-B5A3-F393-E0A9-E50E24DCCA9E - used to send data with  "NOTIFY"

   The design of creating the BLE server is:
   1. Create a BLE Server
   2. Create a BLE Service
   3. Create a BLE Characteristic on the Service
   4. Create a BLE Descriptor on the characteristic
   5. Start the service.
   6. Start advertising.

   In this example rxValue is the data received (only accessible inside that function).
   And txValue is the data to be sent, in this example just a byte incremented every second. 
*/
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

BLEServer *pServer = NULL;
BLECharacteristic * pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint8_t txValue = 0;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

// Actuator Variables

const int a1a = 13;
const int a1b = 14;
const int b1a = 16;
const int b1b = 17;
unsigned long previousMillis = 0;

double slid_freq = 2;
double tap_freq = 7;
double vib_freq = 40;
double default_dura = 5000;


void reset() {
  digitalWrite(a1a, LOW);
  digitalWrite(a1b, LOW);
  digitalWrite(b1a, LOW);
  digitalWrite(b1b, LOW);
}

void slide(int l_on, int r_on) {
  digitalWrite(a1a, l_on);
  digitalWrite(a1b, LOW);
  digitalWrite(b1a, r_on);
  digitalWrite(b1b, LOW);
}

void tap_vib(int l_on, int r_on) {
  digitalWrite(a1a, l_on);
  digitalWrite(a1b, 1 - l_on);
  digitalWrite(b1a, r_on);
  digitalWrite(b1b, 1 - r_on);
}

void dragging(double frequency, double duration) {
  reset();
  double interval = (1.0 / (frequency * 2)) * 1000.0;
  unsigned long startTime = millis();
  unsigned long endTime = startTime;
  int l_on = LOW;
  int r_on = LOW;
  
  while ((endTime - startTime) <= duration)
  {    
    if (millis() >= previousMillis + interval) {
      previousMillis = millis();
      l_on = l_on == LOW ? HIGH : LOW;
      r_on = 1 - l_on;
      slide(l_on, r_on);
      delay(10);
    }
    endTime = millis();
    delay(10);
  }
  reset();
}

void tapping_or_vibrating(double frequency, double duration) {
  reset();
  double interval = (1.0 / (frequency * 2)) * 1000.0;
  unsigned long startTime = millis();
  unsigned long endTime = startTime;
  int l_on = LOW;
  int r_on = LOW;
  while ((endTime - startTime) <= duration) 
  {
    if (millis() >= previousMillis + interval) {
      previousMillis = millis();
      r_on = l_on = l_on == LOW ? HIGH : LOW;
      tap_vib(l_on, r_on);
      delay(10);
    }
    endTime = millis();
    delay(10);
  }
  reset();
}



class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      Serial.println("MyserverCallbacks onConnect");
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      Serial.println("MyServerCallbacks onDisconnect");
      deviceConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();

      if (rxValue.length() > 0) {
//        reset();
        Serial.println("*********");
        Serial.print("Received Value: ");
        for (int i = 0; i < rxValue.length(); i++)
          Serial.print(rxValue[i]);

        if(rxValue == "D") 
        {
          Serial.println("Receive notification, Dragging!");
          dragging(slid_freq, default_dura);
        } 
        else if (rxValue == "T") 
        {
          Serial.println("Receive notification, Tapping!");
          tapping_or_vibrating(tap_freq, default_dura);
        } else if (rxValue == "V")
        {
          Serial.println("Receive notification, Vibrating!");
          tapping_or_vibrating(vib_freq, default_dura);
        }
        
        Serial.println();
        Serial.println("*********");      
      }
    }
};


void setup() {
  Serial.begin(115200);

  pinMode(a1a, OUTPUT);
  pinMode(a1b, OUTPUT);
  pinMode(b1a, OUTPUT);
  pinMode(b1b, OUTPUT);

  pinMode(12, OUTPUT);

  // Create the BLE Device
  BLEDevice::init("ESP32");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pTxCharacteristic = pService->createCharacteristic(
										CHARACTERISTIC_UUID_TX,
										BLECharacteristic::PROPERTY_NOTIFY
									);
                      
  pTxCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic * pRxCharacteristic = pService->createCharacteristic(
											 CHARACTERISTIC_UUID_RX,
											BLECharacteristic::PROPERTY_WRITE
										);

  pRxCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}

void loop() {

    if (deviceConnected) {
        pTxCharacteristic->setValue(&txValue, 1);
        pTxCharacteristic->notify();
        txValue++;
		delay(10); // bluetooth stack will go into congestion, if too many packets are sent
	}

    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
		// do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
}
