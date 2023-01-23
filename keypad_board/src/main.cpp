#include <Arduino.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#define relayPin 2

//i2c address is 0x27
LiquidCrystal_I2C lcd(0x27, 16, 2);

const byte ROWS = 4; 
const byte COLS = 4; 

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {12, 11, 10, 9}; 
byte colPins[COLS] = {8, 7, 6, 5}; 
char TheCombi[6] = {'7', '2', 'A', 'D', 'D', 'C'};

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

void cleanState(){
  lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("Enter PIN:");
  lcd.setCursor(0,1);
}

void wrongLiao(){
  lcd.setCursor(0,1);
  lcd.print("Wrong Combi!");
  delay(2000);
}

void winLiao(){
  lcd.setCursor(0,1);
  lcd.print("Correct Combi!");
  delay(2000);
}

void setup() {
  // put your setup code here, to run once:
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);
  Serial.begin(115200);

  lcd.begin();
	lcd.backlight();
  cleanState();
}

uint8_t charCount = 0;
char InputtedKey[6];
bool wrongFlag;

void loop() {
  // put your main code here, to run repeatedly:
  //digitalWrite(13, !digitalRead(13));
  //delay(500);
  
  char customKey = customKeypad.getKey();
  
  if (customKey){
    lcd.print(customKey);

    InputtedKey[charCount] = customKey;
    charCount++;
    //Serial.println(charCount);
    if (charCount == 6){
      wrongFlag = false;

      for (uint8_t index = 0; index < charCount; index++){
        if (InputtedKey[index] != TheCombi[index]){
          Serial.print("inputted key: ");
          Serial.println(InputtedKey[index]);
          Serial.print("The actual combi key: ");
          Serial.println(TheCombi[index]);

          wrongFlag = true;
        }
        /*else{
          wrongFlag = false;
        }*/
      }

      if (wrongFlag){ //incorrect input
        digitalWrite(relayPin, LOW);
        wrongLiao();
      }
      else if (!wrongFlag){ //correct input
        digitalWrite(relayPin, HIGH);
        winLiao();
      }
      cleanState();
      charCount = 0;
     // wrongFlag = false;
    }
  } 
} 