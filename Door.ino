
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <SoftwareSerial.h>

#define SS_PIN 10
#define RST_PIN 9
SoftwareSerial BTSerial(4,7);
#define  LA3 220.00
#define F3  174.61
#define C4  261.63

#define offset 2

// DURATION OF THE NOTES
#define BPM 120    //  you can change this value changing all the others
#define H 2*Q //half 2/4
#define Q 60000/BPM //quarter 1/4 
#define E Q/2   //eighth 1/8
#define S Q/4 // sixteenth 1/16
#define W 4*Q // whole 4/4

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

Servo myservo;  // create servo object to control a servo
int pos = 0;    // variable to store the servo position
bool dopen;

int pushButton = 2;
bool sbut;
int LedPin = 5;
int greenLed = 6;
byte blueCard[4] =  {0xD5, 0xD3, 0xA1, 0x2C};
byte IlliaCard[4] = {0xE5, 0x5E, 0x05, 0x9C};
byte AntonCard[4] = {0x05, 0x00, 0x5F, 0x9C};
byte DimaCard[4] =  {0x15, 0x56, 0x60, 0x9C};
byte MaksCard[4] =  {0x53, 0x7D, 0x5F, 0x00};

void setup() 
{
  Serial.begin(9600);
  BTSerial.begin(38400);
  myservo.attach(3);
  dopen = true;
  sbut = false;

  SPI.begin();
  rfid.PCD_Init();
  pinMode(LedPin, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(pushButton, INPUT);
  
  digitalWrite(LedPin, LOW);
  digitalWrite(greenLed, 150);
//  blueCard = {0xD5, 0xD3, 0xA1, 0x2C};
  //IlliaCard = {0xE5, 0x5E, 0x05, 0x9C};
 // AntonCard = {0x05, 0x00, 0x5F, 0x9C};
//  DimaCard = {0x15, 0x56, 0x60, 0x9C};
}

void loop() 
{
    int val = digitalRead(pushButton);
   if (BTSerial.available()) 
    {
      if(BTSerial.read()=='1')
        MoveDoor();
        return;
    }
    if (val == HIGH && sbut == false) 
    {
      MoveDoor();
      sbut = true;
      return;
    }
    if (val == HIGH && sbut == true) 
    {
      MoveDoor();
      sbut = false;
      return;
    }

  if ( !rfid.PICC_IsNewCardPresent()) //break loop if no cards available
    return;
    
  if ( !rfid.PICC_ReadCardSerial())   //break loop if card ID isn't read
    return;
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));
  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
      piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
      piccType != MFRC522::PICC_TYPE_MIFARE_4K) 
    {
      return;
    }
  if (pickCard(blueCard) || pickCard(AntonCard) || pickCard(IlliaCard) || pickCard(DimaCard) || pickCard(MaksCard)) 
  {
    MoveDoor();
  } 
  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}


void MoveDoor() 
{
  myservo.attach(3);
  if (dopen) 
  {
    digitalWrite(LedPin, LOW);
    myservo.write(180);
  } 
  else 
  {
    digitalWrite(LedPin, HIGH);
    myservo.write(0);
  }
  dopen = !dopen;
  playSound();
  myservo.detach();
}

bool pickCard(byte CurPICC[4]) 
{
  if (rfid.uid.uidByte[0] == CurPICC[0] &&
      rfid.uid.uidByte[1] == CurPICC[1] &&
      rfid.uid.uidByte[2] == CurPICC[2] &&
      rfid.uid.uidByte[3] == CurPICC[3]) 
      {
        return true;
      } 
      else 
      {
        return false;
      }
}
/**
   Helper routine to dump a byte array as hex values to Serial.
*/
void printHex(byte *buffer, byte bufferSize) 
{
  for (byte i = 0; i < bufferSize; i++) 
  {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

void playSound() 
{
  tone(8, F3, S);
  delay(1+S);
  tone(8, LA3, S);
  delay(1 + S);
}

