#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9
#define SS_PIN 10

MFRC522 mfrc522(SS_PIN, RST_PIN);

int xPin = A0;
int yPin = A1;
int led1Pin = 2;
int led2Pin = 3;
int led3Pin = 4;
int led4Pin = 5;
int buttonPin = 6;
int buzzerPin = 7;

int xVal;
int yVal;
int buttonState;

bool rfidAuthorized = false;
bool isLoggedIn = false;
// RFID Valid uid
byte validUID[] = {0xxx, 0xxx, 0xxx, 0xxx};

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);
  pinMode(led3Pin, OUTPUT);
  pinMode(led4Pin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  Serial.println("Scan kartu RFID anda untuk mengaktifkan joystick..");
}

void loop() {
  if(!rfidAuthorized){
    checkRFID();
    return;
  }

  xVal = analogRead(xPin);
  yVal = analogRead(yPin);
  buttonState = digitalRead(buttonPin);

  Serial.print("X: ");
  Serial.print(xVal);
  Serial.print(" | Y: ");
  Serial.print(yVal);
  Serial.print(" | Button: ");
  Serial.println(buttonState);

  if(xVal > 600){
    digitalWrite(led1Pin, HIGH);
  }else{
    digitalWrite(led1Pin, LOW);
  }
  
  if(xVal < 300) {
    digitalWrite(led2Pin, HIGH);
  }else{
    digitalWrite(led2Pin, LOW);
  }

  if(yVal > 600){
    digitalWrite(led3Pin, HIGH);
  }else{
    digitalWrite(led3Pin, LOW);
  }
  
  if(yVal < 300) {
    digitalWrite(led4Pin, HIGH);
  }else{
    digitalWrite(led4Pin, LOW);
  }

  if(buttonState == 0){
    tone(buzzerPin, 1000);
    digitalWrite(led1Pin, HIGH);
    digitalWrite(led2Pin, HIGH);
    digitalWrite(led3Pin, HIGH);
    digitalWrite(led4Pin, HIGH);
    for (int i = 0; i < 4; i++) {

    }

    delay(1000);
    noTone(buzzerPin);
  }
  delay(100);

  checkRFID();
}

void checkRFID(){
  if(!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()){
    return;
  }

  // Periksa apakah uid RFID cocok
  if (mfrc522.uid.size != sizeof(validUID)) {
    Serial.println("RFID tidak valid! Ukuran UID tidak cocok.");
    playInvalidBeep();
    return;
  }

  bool isAuthorized = true;
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] != validUID[i]) {
      isAuthorized = false;
      break;
    }
  }

  if (isAuthorized) {
    if (!isLoggedIn) {
      Serial.println("RFID valid! Joystick diaktifkan.");
      playValidBeep();
      rfidAuthorized = true;
      isLoggedIn = true;
    } else {
      Serial.println("RFID valid! Joystick dinonaktifkan.");
      playLogoutBeep();
      rfidAuthorized = false;
      isLoggedIn = false;
    }
  } else {
    Serial.println("RFID tidak valid! Coba lagi.");
    playInvalidBeep();
    rfidAuthorized = false;
  }

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}

void playValidBeep() {
  tone(buzzerPin, 1000);
  delay(1000);
  noTone(buzzerPin);
}

void playInvalidBeep() {
  for (int i = 0; i < 2; i++) {
    tone(buzzerPin, 1000);
    delay(200);
    noTone(buzzerPin);
    delay(200); 
  }
}
void playLogoutBeep() {
  for (int i = 0; i < 3; i++) {
    tone(buzzerPin, 1000);
    delay(200);
    noTone(buzzerPin);
    delay(200); 
  }
}
