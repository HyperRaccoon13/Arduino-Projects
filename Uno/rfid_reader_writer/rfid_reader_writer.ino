
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN); 
MFRC522::MIFARE_Key key; 
byte nuidPICC[4];


LiquidCrystal_I2C lcd(0x27, 16, 2);


const int buttonPinWrite = 2;
const int buttonPinRead = 3;
const int buttonPinSelect = 6;

int buttonStateWrite = 0;
int buttonStateRead = 0;    
int buttonStateSelect = 0;
String baseMessage = "Read | Write";
String readMessage = "Ready to read";
String readMessageCard = ".. .. .. ..";
String writeMessage = "Ready to write";
String UID = "";





void setup() {
  //Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();


  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(0, 0);

  pinMode(buttonPinWrite, INPUT_PULLUP);
  pinMode(buttonPinRead, INPUT_PULLUP);
  pinMode(buttonPinSelect, INPUT_PULLUP);
}



byte hexCharToByte(char c) {
  if ('0' <= c && c <= '9') {
    return c - '0';
  }
  if ('A' <= c && c <= 'F') {
    return c - 'A' + 10;
  }
  if ('a' <= c && c <= 'f') {
    return c - 'a' + 10;
  }
  return 0; // Invalid character
}


void ReadCard() {
  if ( ! rfid.PICC_IsNewCardPresent()) {
     return;
  }
  else { UID = ""; }

  if ( ! rfid.PICC_ReadCardSerial())
    return;

  for (byte i = 0; i < rfid.uid.size; i++) {
    UID += String(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
    UID += String(rfid.uid.uidByte[i], HEX);
  }

}

void WriteCard() {
  if ( ! rfid.PICC_IsNewCardPresent() || ! rfid.PICC_ReadCardSerial() ) {
    delay(50);
    return;
  }
 
  String uidString = UID;  // Convert UID string to String object
  byte newUid[4];  

  for (int i = 0; i < 4; ++i) {
    String hexByte = uidString.substring(2 * i, 2 * i + 2); // Extract each pair of characters
    newUid[i] = (byte) strtol(hexByte.c_str(), NULL, 16); // Convert hex string to byte
    Serial.print(newUid[i]);
  }
  
  if ( rfid.MIFARE_SetUid(newUid, (byte)4, true) ) {
    lcd.setCursor(0, 1);
    lcd.print("Changed UID");
  }
}


void HighLigher(int startPos, int length, String origin) {
  lcd.clear();
  int endPos = startPos + length;
  lcd.setCursor(2, 0);
  lcd.print(origin.substring(0, startPos));
  for (int i = startPos; i < endPos; ++i) {
    lcd.print((char)255);
  }
  lcd.print(origin.substring(endPos));
  delay(500);
}


int currentOption = 0;


void loop() {

  buttonStateWrite = digitalRead(buttonPinWrite);
  buttonStateRead = digitalRead(buttonPinRead);
  buttonStateSelect = digitalRead(buttonPinSelect);

  if (buttonStateRead == LOW) {
    currentOption = 1;
    digitalWrite(buttonPinWrite, HIGH);
  } 

  else if (buttonStateWrite == LOW) {
    currentOption = 2;
    digitalWrite(buttonPinRead, HIGH);
  } 
  
  else if (buttonStateSelect == LOW && currentOption == 1) {
    currentOption = 3;
  }

  else if (buttonStateSelect == LOW && currentOption == 2) {
    currentOption = 4;
    
  }


  switch (currentOption) {
    case 1: // Read
      HighLigher(0, 4, baseMessage);
      lcd.setCursor(2, 0);
      lcd.print(baseMessage);
      break;
    case 2: // Write
      HighLigher(7, 5, baseMessage);
      lcd.setCursor(2, 0);
      lcd.print(baseMessage);
      break;

    case 3: // Reading handler
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(readMessage);
      lcd.setCursor(0, 1);
      lcd.print(UID);
      ReadCard();


      



      break;

    case 4:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(writeMessage);
      WriteCard();
      break;

    default: // Line
      HighLigher(5, 1, baseMessage);
      lcd.setCursor(2, 0);
      lcd.print(baseMessage);
    }

  
  

  delay(500);
}
