#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <Adafruit_NeoPixel.h>
#include "motorControl.h"

#define PIN 15

/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-esp32-arduino-ide/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/


// Structure example to receive data
// Must match the sender structure

typedef struct struct_message {
    char a[32];
    uint16_t right_value;
    uint16_t left_value;
    String d;
    bool switchLightOn;
    bool txJustSwitchedOn;
} struct_message;

// Create a struct_message called myData
struct_message myData;
uint32_t cMillisX, cMillisY;
uint32_t pMillisX = 0, pMillisY = 0;
uint16_t intervalX = 5, intervalY = 7;
bool receivedFlag = false;
bool extraPowah = false;

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  receivedFlag = true;
  memcpy(&myData, incomingData, sizeof(myData)); /*
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Char: ");
  Serial.println(myData.a);
  Serial.print("X value: ");
  Serial.println(myData.right_value);
  Serial.print("Y value: ");
  Serial.println(myData.left_value);
  Serial.print("String: ");
  Serial.println(myData.d);
  Serial.print("txJustSwitchedOn?: ");
  Serial.println(myData.txJustSwitchedOn);
  Serial.println();*/
}
 

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  pinMode(PIN,OUTPUT);
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

 // Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);   
  motors_init();
  disconnected_state();

}
 
void loop() {
  if (myData.switchLightOn){
    digitalWrite(PIN, HIGH);
    extraPowah = true;
  }
  else if (!myData.switchLightOn){
    digitalWrite(PIN,LOW);
    extraPowah = false;
  }
  //Serial.print("switchLighton: ");
  //Serial.println(myData.switchLightOn);

  cMillisX = cMillisY = millis();
  if (myData.txJustSwitchedOn || !receivedFlag){
    disconnected_state();
  }  

  else {
    if (cMillisX - pMillisX >= intervalX){
      if (extraPowah)
        read_x_axis_adc(myData.right_value, 4010);

      else if (!extraPowah)
        read_x_axis_adc(myData.right_value, 3099);
      pMillisX = cMillisX;
    }

    if (cMillisY - pMillisY >= intervalY){
        read_y_axis_adc(myData.left_value);
        pMillisY = cMillisY;
    }
  }
}