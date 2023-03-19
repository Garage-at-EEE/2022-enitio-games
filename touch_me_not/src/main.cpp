#include <Arduino.h>
#include <FastLED.h>
#include <avr/wdt.h>

#define BUTTON_PIN 8
#define NUM_LEDS 69
#define DATA_PIN 3
#define BUZZER_PIN 5

#define START_PIN 13
#define END_PIN 12

#define RELAY_PIN 2
CRGB leds[NUM_LEDS];

void playTone(uint16_t delaY){
  tone(BUZZER_PIN, 440, 500);
  delay(delaY);
  tone(BUZZER_PIN, 510, 500);
  delay(delaY);
  tone(BUZZER_PIN, 585, 500);
  delay(delaY);
  noTone(BUZZER_PIN);
}

void playVictoryTone(uint16_t delaY){
  tone(BUZZER_PIN, 523, 500);
  delay(delaY);
  tone(BUZZER_PIN, 494, 500);
  delay(delaY);
  tone(BUZZER_PIN, 440, 500);
  delay(delaY);
  noTone(BUZZER_PIN);
}
void setup() {
  //wdt_enable(WDTO_500MS);
  // put your setup code here, to run once:
 // pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(START_PIN, INPUT_PULLUP);
  pinMode(END_PIN, INPUT_PULLUP);
  pinMode(RELAY_PIN, OUTPUT);
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // GRB ordering is assumed
  Serial.begin(115200);
  playTone(150);

  digitalWrite(RELAY_PIN, LOW);
  for (uint8_t i = 0; i< NUM_LEDS; i++){
    leds[i] = CRGB::Green;
  }
  FastLED.show();
}

bool playerHasScrewedUp = false;

void loop() {
  // put your main code here, to run repeatedly:

  if (!digitalRead(BUTTON_PIN)){
    playerHasScrewedUp = true;
    Serial.println("button presed");
    analogWrite(BUZZER_PIN, 100);
    //delay(10);
  }

  else {  
    analogWrite(BUZZER_PIN, 0);
   // delay(10);
  }

  if (playerHasScrewedUp){
    for (uint8_t i = 0; i< NUM_LEDS; i++){
      leds[i] = CRGB::Red;
    }
    FastLED.show();
    digitalWrite(RELAY_PIN, LOW);

    Serial.println("player has screwed up");
      if (!digitalRead(START_PIN)){
        playerHasScrewedUp = false;
        Serial.println("Player has been unscrewed;");
        for (uint8_t i = 0; i< NUM_LEDS; i++){
          leds[i] = CRGB::Green;
        }
        FastLED.show();
        playTone(200);
    }
  }

  else if (!playerHasScrewedUp){
    

    if (!digitalRead(END_PIN)){
      for (uint8_t i = 0; i< NUM_LEDS; i++){
      leds[i] = CRGB::Blue;
      FastLED.show();
    }
    Serial.println("Player has finished!");
    digitalWrite(RELAY_PIN,HIGH);
    playVictoryTone(100);
    }
  }
  //wdt_reset();
  
}