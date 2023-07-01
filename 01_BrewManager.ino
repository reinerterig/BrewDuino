#include <ArduinoBLE.h>

BLEService brevilleService("19B10000-E8F2-537E-4F6C-D104768A1214"); // BLE LED Service

// Current Servo Position Characteristic - custom 128-bit UUID, read by central
BLEFloatCharacteristic currentServoPositionCharacteristic("19B10010-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);

// Set Servo Position Characteristic - custom 128-bit UUID, writable by central
BLEFloatCharacteristic setServoPositionCharacteristic("19B10011-E8F2-537E-4F6C-D104768A1214", BLEWrite);

// Servo Resistance Characteristic - custom 128-bit UUID, read by central
BLEFloatCharacteristic servoResistanceCharacteristic("19B10012-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);

// Power Button Characteristic - custom 128-bit UUID, writable by central
BLEBoolCharacteristic powerButtonCharacteristic("19B10013-E8F2-537E-4F6C-D104768A1214", BLEWrite);

// Brew Button Characteristic - custom 128-bit UUID, writable by central
BLEBoolCharacteristic brewButtonCharacteristic("19B10014-E8F2-537E-4F6C-D104768A1214", BLEWrite);



const int ledPin = LED_BUILTIN; // pin to use for the LED
double currentServoPosition = 0;
double setServoPosition = 0;
double servoResistance = 0;
bool powerButton = false;
bool brewButton = false;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // set LED pin to output mode
  pinMode(ledPin, OUTPUT);

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");

    while (1);
  }

  // set advertised local name and service UUID:
  String localName = "MKR WiFi 1010";
  BLE.setLocalName(localName.c_str()); //Setting a name that will appear when scanning for Bluetooth® devices 
  Serial.println(localName); //Print the local name to the Serial Monitor
  BLE.setAdvertisedService(brevilleService);

  // add the characteristic to the service
  brevilleService.addCharacteristic(currentServoPositionCharacteristic);
  brevilleService.addCharacteristic(setServoPositionCharacteristic);
  brevilleService.addCharacteristic(servoResistanceCharacteristic);
  brevilleService.addCharacteristic(powerButtonCharacteristic);
  brevilleService.addCharacteristic(brewButtonCharacteristic);
  // add service
  BLE.addService(brevilleService);

  // set the initial value for the characeristic:
  //switchCharacteristic.writeValue("off");

  // start advertising
  BLE.advertise();

  Serial.println("Bluetooth device active, waiting for connections...");

  // print the UUID
  Serial.println("The UUID of the Service is: ");
  Serial.println(brevilleService.uuid());
}

unsigned long previousMillis = 0; 
const long interval = 1000;  
bool ledState = false;

void loop() {
  // wait for a BLE central
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());
    
    // while the central is still connected to peripheral:
    while (central.connected()) {

      // Check if it's time to blink the LED
      unsigned long currentMillis = millis();
      if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        // if the LED is off turn it on and vice versa:
        if (ledState == false) {
          digitalWrite(ledPin, HIGH); // turn the LED on
          //switchCharacteristic.writeValue("on"); // sends the string 'on' over Bluetooth
          ledState = true;
        } else {
          digitalWrite(ledPin, LOW); // turn the LED off
          //switchCharacteristic.writeValue("off"); // sends the string 'off' over Bluetooth
          ledState = false;
        }
      }
      
      // if the setServoPosition characteristic has been written to by a central:
    if (setServoPositionCharacteristic.written()) {
    // get the value written by the central:
     setServoPosition = setServoPositionCharacteristic.value();
    Serial.print("Set Servo: ");
    Serial.println(setServoPosition, 1); // 1 for single decimal precision
}

      // if the powerButton characteristic has been written to by a central:
  if (powerButtonCharacteristic.written()) {
    // get the value written by the central:
     powerButton = powerButtonCharacteristic.value();
    Serial.print("Power Button: ");
    Serial.println(powerButton ? "true" : "false");
}

      // if the brewButton characteristic has been written to by a central:
      if (brewButtonCharacteristic.written()) {
    // get the value written by the central:
     brewButton = brewButtonCharacteristic.value();
    Serial.print("Brew Button: ");
    Serial.println(brewButton ? "true" : "false");
}


    }
    // when the central disconnects, print it out:
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
}
