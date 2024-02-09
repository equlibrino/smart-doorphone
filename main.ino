#include <Arduino.h>
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ESP32) || defined(ARDUINO_ARCH_RP2040)
#include <WiFi.h>
#endif

#include "SinricPro.h"
#include "SinricProSwitch.h"

#define WIFI_SSID         "null"  
#define WIFI_PASS         "null" 
#define APP_KEY           "null"  
#define APP_SECRET        "null" 
#define SWITCH_ID_1       "null" // 2 Swıtch

#if defined(ESP8266)
#define RELAYPIN_1        14
#define RELAYPIN_2        12
#endif

#define BAUD_RATE         115200 // Change baudrate to your need

bool onPowerState(const String &deviceId, bool &state) {
  Serial.printf("Device turned %s", state ? "on" : "off");
  digitalWrite(RELAYPIN_1, state ? LOW : HIGH);

  if (state) { // if power is on
    delay(200);
    digitalWrite(RELAYPIN_2, LOW); // Open RELAYPIN_2
    delay(400); 
    digitalWrite(RELAYPIN_2, HIGH); // Close RELAYPIN_2
    delay(10);
    digitalWrite(RELAYPIN_1, HIGH); // Close RELAYPIN_1
  }
  
  return true; // request handled properly
}

// setup function for WiFi connection
void setupWiFi() {
  Serial.printf("\r\n[Wifi]: Connecting");

#if defined(ESP8266)
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  WiFi.setAutoReconnect(true);
#elif defined(ESP32)
  WiFi.setSleep(false);
  WiFi.setAutoReconnect(true);
#endif

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.printf(".");
    delay(250);
  }

  Serial.printf("connected!\r\n[WiFi]: IP-Address is %s\r\n", WiFi.localIP().toString().c_str());
}

// setup function for SinricPro
void setupSinricPro() {
  // add devices and callbacks to SinricPro
  pinMode(RELAYPIN_1, OUTPUT); 
  pinMode(RELAYPIN_2, OUTPUT);

  SinricProSwitch& mySwitch = SinricPro[SWITCH_ID_1];
  mySwitch.onPowerState(onPowerState);
  
  // setup SinricPro
  SinricPro.onConnected([]() {
    Serial.printf("Connected to SinricPro\r\n");
  });
  SinricPro.onDisconnected([]() {
    Serial.printf("Disconnected from SinricPro\r\n");
  });

  SinricPro.begin(APP_KEY, APP_SECRET);
}

// main setup function
void setup() {
  Serial.begin(BAUD_RATE); Serial.printf("\r\n\r\n");
  setupWiFi();
  setupSinricPro();
  digitalWrite(RELAYPIN_1, HIGH);
  digitalWrite(RELAYPIN_2, HIGH);

}

void loop() {
  SinricPro.handle();
}
