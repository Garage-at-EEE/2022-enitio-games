#include <Arduino.h>
#include <esp_now.h>
#include "WiFi.h"
#define x_pin 34
#define y_pin 35
#define JUST_STARTED_UP 1000
#define TOUCHPAD 13


//uint8_t broadcastAddress[] = {0x84, 0xCC, 0xA8, 0x7A, 0xE4, 0xD4}; //84:CC:A8:7A:E4:D4
// old address: 0x84, 0xCC, 0xA8, 0x2C, 0x32, 0x18

uint8_t broadcastAddress[] = {0x78, 0xE3, 0x6D, 0x17, 0x1B, 0xD4}; //84:CC:A8:7A:E4:D4
//set B broadcast address: 78 E3 6D 17 1B D4
uint16_t XpotValue, YpotValue;
uint16_t interval1 = 5;
uint32_t cMillis1;
uint32_t pMillis1 = 0;

  // Structure example to send data
  // Must match the receiver structure
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

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  // put your setup code here, to run once:
  // Init Serial Monitor
  Serial.begin(115200);
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  myData.switchLightOn = false;
}

uint32_t cMillis11;
uint16_t interval2 = 50;
uint32_t pMillis11 = 0, ppMillis11 = 0;
uint8_t value, pvalue;
bool hiToLow = false;

void loop() {
  pvalue = touchRead(TOUCHPAD);
  if (touchRead(TOUCHPAD) > 15){
      cMillis11 = millis();
  }
  if (cMillis11 - pMillis11 >= interval2){
    value = touchRead(TOUCHPAD);
    if (value < 15){
      hiToLow = true;
      pMillis11 = cMillis11;
    }
  }

  if (hiToLow){
      myData.switchLightOn = !myData.switchLightOn;
      hiToLow = false;
  }
  Serial.println(myData.switchLightOn);
  // put your main code here, to run repeatedly:
  // Set values to send

  XpotValue = analogRead(x_pin);
  YpotValue = analogRead(y_pin);

  if (YpotValue > 3711){
    YpotValue = 3675;
  }

  
  Serial.println("Right pot value: ");
  Serial.println(XpotValue);
    Serial.println("Left pot value: ");
  Serial.println(YpotValue);
  
  strcpy(myData.a, "THIS IS A CHAR");
  myData.right_value = XpotValue;
  myData.left_value = YpotValue;
  myData.d = "Hello";


  cMillis1 = millis();

  if (cMillis1 < JUST_STARTED_UP){
    myData.txJustSwitchedOn = true;
  }
  else {
    myData.txJustSwitchedOn = false;
  }

  if (cMillis1 - pMillis1 >= interval1){
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));   
    if (result == ESP_OK) {
      Serial.print(myData.txJustSwitchedOn);
      Serial.println("Sent with success");
    }
    else {
      //Serial.println("Error sending the data");
    }
    pMillis1 = cMillis1;
  }
  // Send message via ESP-NOW
  
}