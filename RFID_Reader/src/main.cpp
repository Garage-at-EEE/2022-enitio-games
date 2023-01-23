/*
 * --------------------------------------------------------------------------------------------------------------------
 * Example sketch/program showing how to read data from a PICC to serial.
 * --------------------------------------------------------------------------------------------------------------------
 * This is a MFRC522 library example; for further details and other examples see: https://github.com/miguelbalboa/rfid
 * 
 * Example sketch/program showing how to read data from a PICC (that is: a RFID Tag or Card) using a MFRC522 based RFID
 * Reader on the Arduino SPI interface.
 * 
 * When the Arduino and the MFRC522 module are connected (see the pin layout below), load this sketch into Arduino IDE
 * then verify/compile and upload it. To see the output: use Tools, Serial Monitor of the IDE (hit Ctrl+Shft+M). When
 * you present a PICC (that is: a RFID Tag or Card) at reading distance of the MFRC522 Reader/PCD, the serial output
 * will show the ID/UID, type and any data blocks it can read. Note: you may see "Timeout in communication" messages
 * when removing the PICC from reading distance too early.
 * 
 * If your reader supports it, this sketch/program will read all the PICCs presented (that is: multiple tag reading).
 * So if you stack two or more PICCs on top of each other and present them to the reader, it will first output all
 * details of the first and then the next PICC. Note that this may take some time as all data blocks are dumped, so
 * keep the PICCs at reading distance until complete.
 * 
 * @license Released into the public domain.
 * 
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 *
 * More pin layouts for other boards can be found here: https://github.com/miguelbalboa/rfid#pin-layout
 */

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9          // Configurable, see typical pin layout above
#define SS_PIN          10         // Configurable, see typical pin layout above
#define RED_LED 2
#define GREEN_LED 3
#define RELAY_OUTPUT 4

MFRC522 rfid(SS_PIN, RST_PIN);  // Create MFRC522 instance

MFRC522::MIFARE_Key key; 

// Init array that will store new NUID 
byte nuidPICC[4];
uint8_t index = 0;
bool wrongLiao;

uint32_t timeSinceGameEnd =  0;

void printHex(byte *buffer, byte bufferSize) {
	for (byte i = 0; i < bufferSize; i++) {
		Serial.print(buffer[i] < 0x10 ? " 0" : " ");
		Serial.print(buffer[i], HEX);
	}
}
void resetAll(){
	index = 0;
	wrongLiao = false;
}

void setup() {
	Serial.begin(115200);
	Serial.println('nabei');
	pinMode(RED_LED, OUTPUT);
	pinMode(GREEN_LED, OUTPUT);
	pinMode(RELAY_OUTPUT, OUTPUT);
	SPI.begin(); // Init SPI bus
	rfid.PCD_Init(); // Init MFRC522 

	for (byte i = 0; i < 6; i++) {
		key.keyByte[i] = 0xFF;
	}
	wrongLiao = false;
}

byte UID[3][4];
byte ActualID[3][4] = {
	{0xE0, 0x43, 0xDD, 0xA4},
	{0xF2, 0x35, 0x32, 0x2E},
	{0xA0, 0x50, 0xDF, 0xA4}
};


//actual ID for set 2
/*byte ActualID[3][4] = {
	{0x66, 0xCF, 0x29, 0x25},
	{0xC5, 0x52, 0x3D, 0x23},
	{0xD5, 0xF2, 0xB3, 0x23}
};*/

bool gameEnded = false;
uint32_t pMillis1 = 0;
uint32_t pMillis2 = 0;
uint32_t cMillis1, cMillis2;
uint8_t interval1 = 100;
uint16_t interval2 = 3000;

bool flashRedLED = false;
bool flashGreenLED = false;
bool onning = false;
uint32_t pMillis3 = 0;
uint32_t cMillis3;
uint32_t interval3 = 15000;//15 seconds before relay cuts

void loop() {
	cMillis1 = millis();
	cMillis2 = millis();

	if (cMillis1 - pMillis1 >= interval1){
		if (flashGreenLED){
			onning = true;
			digitalWrite(GREEN_LED, !digitalRead(GREEN_LED));
		}

		if (flashRedLED){
			onning = true;
			digitalWrite(RED_LED, !digitalRead(RED_LED));
		}
		pMillis1 = cMillis1;
	}

	if (cMillis2 - pMillis2 >= interval2){
		if (onning){
			if (flashGreenLED){
				onning = false;
				digitalWrite(GREEN_LED, LOW);
				flashGreenLED = false;
			}
			if (flashRedLED){
				onning = false;
				digitalWrite(RED_LED, LOW);
				flashRedLED = false;
			}
		}
		pMillis2 = cMillis2;
	}
	// Look for new cards
	if ( ! rfid.PICC_IsNewCardPresent())
		return;

	// Verify if the NUID has been readed
	if ( ! rfid.PICC_ReadCardSerial())
		return;



	for (byte i = 0; i < 4; i++) {
		nuidPICC[i] = rfid.uid.uidByte[i];
		UID[index][i] = nuidPICC[i];
		if (i == 3)
			index++;
	}

   
  	printHex(rfid.uid.uidByte, rfid.uid.size);
	Serial.println();
	rfid.PICC_HaltA();
  	rfid.PCD_StopCrypto1();


	for (uint8_t i = 0; i < 4; i++) {
		if (ActualID[index-1][i] != nuidPICC[i]){
			if (!gameEnded){
				Serial.print("Scanned UID byte: ");
				Serial.println(UID[index-1][i], HEX);
				Serial.print("Correct byte: ");
				Serial.println(nuidPICC[i], HEX);
				Serial.println("Wrong sequence!");
				wrongLiao = true;
			}
		}
		else{
			Serial.println("Flash green LED true");
			//flashGreenLED = true;
		}
	}

	if (wrongLiao){
		flashRedLED = true;
		resetAll();
	}

	else if (!wrongLiao){
		flashGreenLED = true;
	}

	if (index == 3 && !gameEnded){
		Serial.println("game has ended!");
		gameEnded = true;
		pMillis3 = millis();
		digitalWrite(RELAY_OUTPUT, HIGH);
		index = 0;
		delay(interval3);
		digitalWrite(RELAY_OUTPUT, LOW);
		gameEnded = false;
	}

	if (gameEnded){
		timeSinceGameEnd = millis() - pMillis3;
		if (timeSinceGameEnd > 5000) digitalWrite(RELAY_OUTPUT, LOW);
	}

	cMillis3 = millis();
	if(cMillis3 - pMillis3 > interval3 && gameEnded){
		digitalWrite(RELAY_OUTPUT, LOW);
	}

	else if (!gameEnded){
		digitalWrite(RELAY_OUTPUT, LOW);
	}
}



