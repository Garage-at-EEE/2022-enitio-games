#include <FastLED.h>
#include <Bounce2.h>
#include <TM1637Display.h>

// TM1673 display 
#define CLK 5
#define DIO 6

// Buttons
#define BUTTON_PIN_1 3
#define BUTTON_PIN_2 4
#define BUTTON_PIN_3 7
#define BUTTON_PIN_4 2
#define DEBOUNCE_INTERVAL 10

// ARGB LEDs
#define LED_PIN_1 8
#define LED_PIN_2 12
#define LED_PIN_3 13
#define NUM_LEDS 20 // Number of LEDS per strip

// TM1637 display
TM1637Display display(CLK, DIO);
// Hex array for "done"
const uint8_t done[] = {
  SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,          // d
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,  // O
  SEG_C | SEG_E | SEG_G,                          // n
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G           // E
};
// Hex array for "play"
const uint8_t play[] = {
  SEG_A | SEG_B | SEG_G | SEG_E | SEG_F,          // p
  SEG_F | SEG_E | SEG_D,                          // L
  SEG_A | SEG_B | SEG_C | SEG_F | SEG_E | SEG_G,  // A
  SEG_B | SEG_C | SEG_D | SEG_G | SEG_F           // y
};
// Hex array for "fail"
const uint8_t fail[] = {
  SEG_A | SEG_G | SEG_F | SEG_E,                  // F
  SEG_A | SEG_B | SEG_C | SEG_F | SEG_E | SEG_G,  // A
  SEG_B | SEG_C,                                  // I
  SEG_D | SEG_E | SEG_F                           // L
};
const uint8_t fin[] = {
  SEG_A | SEG_G | SEG_F | SEG_E,                  // F
  SEG_B | SEG_C,                                  // I
  SEG_C | SEG_E | SEG_G,                          // n
};
const uint8_t L[] = {
  SEG_F | SEG_E | SEG_D
};

// ARGB LEDs
CRGB led1[NUM_LEDS];
CRGB led2[NUM_LEDS];
CRGB led3[NUM_LEDS];
uint8_t counter = 0;
uint8_t index = 0;
uint8_t ledIndex[3] = {0,0,0}; // The number of lights lit up on the LED strips
int currentLedGoal[3] = {0, 0, 0}; // The number of lights that must be hit
int ledGoals[10][3] =
{
  { 1,  2,  3},
  { 2,  4,  5},
  { 9,  6, 12},
  { 5, 13,  8},
  {15,  3,  9},
  { 8, 17,  6},
  {12, 10, 13},
  {11, 18, 14},
  {17, 16, 19},
  {20, 20, 20}
};

int debugLedGoals[3] = {1, 1, 1};


// Buttons
Bounce dotButton1 = Bounce(); // Leftmost button
Bounce dotButton2 = Bounce(); // Center button
Bounce dotButton3 = Bounce(); // Rightmost button
Bounce dotButton4 = Bounce(); // Reset button

bool timer1Flg; // The intervals between each LED strip lights decrease
bool timer2Flg; // No idea what this is for

// Clock
uint32_t currentTime = 0;
uint32_t lastStateTime = 0;
uint32_t levelTime = 0;
uint32_t passedTime = 0;
uint32_t remainingTime = 0;

// Game 
int toAddScore = 0;
int currentScore = 0;
int currentLevel = 0;
int code = 639;

// Time is set in milliseconds

uint32_t debugLevelTime = 5000; 
uint32_t levelTimes[] = 
{
  50000, 45000, 40000, 35000, 30000, 25000, 20000, 15000, 10000, 10000
};
int scores[] = 
{
  100, 200, 300, 400, 500, 750, 1000, 1500, 2000, 3000
};

bool debugMode = false;
bool isPlay = true; // Game state
bool isWin  = false; // Win state

enum GameState { RESET, WAIT, PLAY, CLEAR, WIN, LOSE };
GameState gameState = RESET;

// Functions
void UpdateTime();
void DecreaseLightLevel();
void HandleInput();
void SetLight();
void CheckGameState();
void SetDisplay();
void Reset();
void Win();
void Lose();
void Play();
void Wait();
void Clear();
void UpdateScore();

void setup() {
  // put your setup code here, to run once:
  cli();
    //set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 5210;// = (16*10^6) / (3*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  //set timer2 interrupt at 8kHz
  TCCR2A = 0;// set entire TCCR2A register to 0
  TCCR2B = 0;// same for TCCR2B
  TCNT2  = 0;//initialize counter value to 0
  // set compare match register for 8khz increments
  OCR2A = 255;// = (16*10^6) / (8000*8) - 1 (must be <256)
  // turn on CTC mode
  TCCR2A |= (1 << WGM21);
  // Set CS21 bit for 8 prescaler
  TCCR2B |= (1 << CS21);   
  // enable timer compare interrupt
  TIMSK2 |= (1 << OCIE2A);
  sei();//allow interrupts
  
  FastLED.addLeds<WS2812, LED_PIN_1, GRB>(led1, NUM_LEDS); 
  FastLED.addLeds<WS2812, LED_PIN_2, GRB>(led2, NUM_LEDS); 
  FastLED.addLeds<WS2812, LED_PIN_3, GRB>(led3, NUM_LEDS); 
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 500);
  FastLED.setBrightness(5);
  FastLED.clear();
  FastLED.show();

  dotButton1.attach(BUTTON_PIN_1,INPUT_PULLUP);
  dotButton1.interval(DEBOUNCE_INTERVAL);
  dotButton2.attach(BUTTON_PIN_2,INPUT_PULLUP);
  dotButton2.interval(DEBOUNCE_INTERVAL);
  dotButton3.attach(BUTTON_PIN_3,INPUT_PULLUP);
  dotButton3.interval(DEBOUNCE_INTERVAL);
  dotButton4.attach(BUTTON_PIN_4,INPUT_PULLUP);
  dotButton4.interval(DEBOUNCE_INTERVAL);

  display.setBrightness(2);
  display.clear();
  
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  if (!debugMode) DecreaseLightLevel();
  HandleInput();
  SetLight();
  CheckGameState();
  SetDisplay();
  UpdateTime();
  Serial.println(gameState);
}

void Reset()
{
  display.clear();
  currentLevel = 0;
  currentScore = 0;
  lastStateTime = millis();
  gameState = RESET;
}

void Clear()
{
  display.clear();
  UpdateScore();
  currentLevel++;
  levelTime = 2000;
  lastStateTime = millis();
  gameState = CLEAR;
}

void Wait()
{
  display.clear();
  ledIndex[0] = 0; 
  ledIndex[1] = 0; 
  ledIndex[2] = 0; 

  if (debugMode)
  {
    currentLedGoal[0] = debugLedGoals[0];
    currentLedGoal[1] = debugLedGoals[1];
    currentLedGoal[2] = debugLedGoals[2];
  }
  else
  {
    currentLedGoal[0] = ledGoals[currentLevel][0];
    currentLedGoal[1] = ledGoals[currentLevel][1];
    currentLedGoal[2] = ledGoals[currentLevel][2];
  }
  lastStateTime = millis();
  levelTime = 5000;
  gameState = WAIT;
}

void Play()
{
  display.clear();
  if (debugMode)
  {
    levelTime = debugLevelTime;
  }
  else
  {
    levelTime = levelTimes[currentLevel];
  }
  lastStateTime = millis();
  gameState = PLAY;
}

void Win()
{
  display.clear();
  lastStateTime = millis();
  levelTime = 4000;
  gameState = WIN;
}

void Lose()
{
  display.clear();
  lastStateTime = millis();
  levelTime = 2000;
  gameState = LOSE;
}

void UpdateTime()
{
  currentTime = millis() - lastStateTime;
  passedTime = currentTime / 1000;
  remainingTime = (levelTime - currentTime + 1000) / 1000;
}

void UpdateScore()
{
  toAddScore = scores[currentLevel];
  if (currentLevel > 1) toAddScore -= ( toAddScore * 0.05 ) * ( passedTime );
  currentScore += toAddScore;
}

void DecreaseLightLevel()
{
  // No need to decrease light levels if we are not playing
  if (gameState != PLAY) return;
  if (timer1Flg)
  {
    if (ledIndex[0] > 0) ledIndex[0]--; // Decrease 1st LED strip downwards by one
    if (ledIndex[1] > 0) ledIndex[1]--; // Decrease 2nd LED strip downwards by one
    if (ledIndex[2] > 0) ledIndex[2]--; // Decrease 3rd LED strip downwards by one
    timer1Flg = false;
  }
}

void HandleInput()
{
  if (gameState == RESET)
  {
    dotButton4.update();
    if (dotButton4.fell()) Wait();
  }
  
  if (gameState == PLAY)
  {
    dotButton1.update();
    dotButton2.update();
    dotButton3.update();
    
    if (dotButton1.fell())
    {
      if (ledIndex[0] < 20) ledIndex[0]++; // Advance 1st LED strip upwards by one
    }
    if (dotButton2.fell())
    {
      if (ledIndex[1] < 20) ledIndex[1]++; // Advace second LED strip upwards by one
    }
    if (dotButton3.fell())
    {
      if (ledIndex[2] < 20) ledIndex[2]++; // Advace second LED strip upwards by one
  }
  }
}

void SetLight()
{
  FastLED.clear();

  // // Set the LEDs to display the lights according to their level
  // for (int i = 0; i < ledIndex[0]; i++)
  // {
  //   led1[i] = CRGB::Red;
  // }
  // for (int i = 0; i < ledIndex[1]; i++)
  // {
  //   led2[i] = CRGB::Green;
  // }
  // for (int i = 0; i < ledIndex[2]; i++)
  // {
  //   led3[i] = CRGB::Yellow;
  // }
  // // If the light level has been reached, display the light in a different colour
  // int index;
  // index = currentLedGoal[0] - 1;
  // if (ledIndex[0] > index)
  // {
  //   led1[index] = CRGB::Blue;
  // }
  // else
  // {
  //   led1[index] = CRGB::Red;
  // }
  // index = currentLedGoal[1] - 1;
  // if (ledIndex[1] > index)
  // {
  //   led2[index] = CRGB::Blue;
  // }
  // else
  // {
  //   led2[index] = CRGB::Green;
  // }
  // index = currentLedGoal[2] - 1;
  // if (ledIndex[2] > index)
  // {
  //   led3[index] = CRGB::Blue;
  // }
  // else
  // {
  //   led3[index] = CRGB::Yellow;
  // }
  int count = NUM_LEDS - 1;
  for (int i = count; i > count - ledIndex[0]; i--) led1[i] = CRGB::Red;
  for (int i = count; i > count - ledIndex[1]; i--) led2[i] = CRGB::Green;
  for (int i = count; i > count - ledIndex[2]; i--) led3[i] = CRGB::Yellow;

  // If the light level has been reached, display the light in a different colour
  int index;
  index = currentLedGoal[0] - 1;
  ledIndex[0] > index ? led1[19 - index] = CRGB::Blue : led1[19 - index] = CRGB::Red;
  index = currentLedGoal[1] - 1;
  ledIndex[1] > index ? led2[19 - index] = CRGB::Blue : led2[19 - index] = CRGB::Green;
  index = currentLedGoal[2] - 1;
  ledIndex[2] > index ? led3[19 - index] = CRGB::Blue : led3[19 - index] = CRGB::Yellow;
  
  
  FastLED.show();
}

void CheckGameState()
{
  // No need to check game state if we are not playing
  if (gameState == WAIT)
  {
    if (currentTime > levelTime) Play();
  }
  else if (gameState == CLEAR)
  {
    if (currentTime > levelTime) Wait();  
  }
  else if (gameState == PLAY)
  {
    // If the time runs out, you lose
    if (currentTime > levelTime) Lose();
    else if (ledIndex[0] == currentLedGoal[0] && ledIndex[1] == currentLedGoal[1] && ledIndex[2] == currentLedGoal[2])
    {
      if (currentLevel < 9) Clear();
      else Win();
    }
  }
  else if (gameState == LOSE)
  {
    if (currentTime > levelTime) Reset();
  }
}

void SetDisplay()
{
  // Set the display according to game state
  switch (gameState)
  {
    case PLAY:
      display.showNumberDec(remainingTime, false, 4, 4);
      break;
    case WAIT:
      if (remainingTime > 4)
      {
        if (currentLevel < 9)
        {
          display.showNumberDec(currentLevel + 1, false, 1, 3);
          display.setSegments(L, 1, 2);
        }
        else
        {
          display.showNumberDec(currentLevel + 1, false, 1, 2);
          display.setSegments(L, 1, 1);
        }
      }
      else if (remainingTime > 1) display.showNumberDec(remainingTime - 1, false, 4, 4);
      else if (remainingTime > 0) display.setSegments(play);
      break;
    case CLEAR:
      display.setSegments(done);
      // if (passedTime < 2) display.setSegments(done);
      // else if (passedTime < 4) display.showNumberDec(currentScore, false, 4, 4);
      break;
    case WIN:
      if (passedTime < 2) display.setSegments(done);
      else display.showNumberDec(code, false, 4, 4);
      // if (passedTime < 2) display.setSegments(done);
      // else display.showNumberDec(currentScore, false, 4, 4);
      break;
    case LOSE:
      display.setSegments(fail);
      // if (passedTime < 2) display.setSegments(fail);
      // else display.showNumberDec(currentScore, false, 4, 4);
      break;
  }
}


ISR(TIMER1_COMPA_vect){//timer1 interrupt 1Hz toggles pin 13 (LED)
//generates pulse wave of frequency 1Hz/2 = 0.5kHz (takes two cycles for full wave- toggle high then toggle low)
  timer1Flg = true;
}
  
ISR(TIMER2_COMPA_vect){//timer1 interrupt 8kHz toggles pin 9
//generates pulse wave of frequency 8kHz/2 = 4kHz (takes two cycles for full wave- toggle high then toggle low)
  timer2Flg = true;
}