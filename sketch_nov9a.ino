#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9

// ✅ Authorized Card UIDs (as seen in Serial Monitor)
String UID1 = "3D 33 E9 00";
String UID2 = "67 21 0F 05";

byte lock = 0;

Servo servo;
LiquidCrystal_I2C lcd(0x27, 16, 2);
MFRC522 rfid(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

  servo.attach(3);
  servo.write(70); // Door starts locked

  SPI.begin();
  rfid.PCD_Init();

  lcd.setCursor(0, 0);
  lcd.print("RFID Door Lock");
  lcd.setCursor(2, 1);
  lcd.print("System Ready");
  delay(2000);
  lcd.clear();
}

void loop() {
  lcd.setCursor(4, 0);
  lcd.print("Welcome!");
  lcd.setCursor(1, 1);
  lcd.print("Tap your card");

  if (!rfid.PICC_IsNewCardPresent()) return;
  if (!rfid.PICC_ReadCardSerial()) return;

  // Read and format UID
  String ID = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) ID += "0";
    ID += String(rfid.uid.uidByte[i], HEX);
    if (i != rfid.uid.size - 1) ID += " ";
  }
  ID.toUpperCase();

  Serial.print("Scanned UID: ");
  Serial.println(ID);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Card Detected");
  delay(1000);

  // If valid card detected
  if (ID == UID1 || ID == UID2) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Access Granted");
    lcd.setCursor(0, 1);
    lcd.print("Door is OPEN");
    servo.write(160); // Unlock door
    delay(5000);      // Wait 5 seconds
    servo.write(70);  // Auto-lock door
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Door Auto-Locked");
    delay(1500);
    lcd.clear();
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Access Denied!");
    delay(1500);
    lcd.clear();
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}