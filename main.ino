#include <LedControl.h>

const int DIN = 11;
const int CLK = 13;
const int CS  = 12;
const int UNITS = 4;  
const int RESET_BTN = 2;
const int INCREMENT_BTN = 7;

const int RED = 3;
const int GREEN = 9;
const int BLUE = 10;
const int BUZZER = 4;

int count = 0;

const byte digits[10][8] = {
  {B00111100,B01100110,B01100110,B01100110,B01100110,B01100110,B01100110,B00111100},
  {B01111110,B00011000,B00011000,B00011000,B00011000,B00011000,B00011100,B00011000},
  {B01111110,B00000110,B00001100,B00011000,B00110000,B01100000,B01100110,B00111100},
  {B00111100,B01100110,B01100000,B01100000,B00111000,B01100000,B01100110,B00111100},
  {B00110000,B00110000,B00110000,B01111110,B00110010,B00110100,B00111000,B00110000},
  {B00111100,B01100110,B01100000,B01100000,B01100000,B00111110,B00000110,B01111110},
  {B00111100,B01100110,B01100110,B01100110,B00111110,B00000110,B01100110,B00111100},
  {B00001100,B00001100,B00001100,B00001100,B00011000,B00110000,B01100000,B01111110},
  {B00111100,B01100110,B01100110,B01100110,B00111100,B01100110,B01100110,B00111100},
  {B00111100,B01100110,B01100000,B01111100,B01100110,B01100110,B01100110,B00111100}
};

const byte smileyFace[8] = {
  B00000000,  // was row 7
  B00111100,  // was row 6
  B01000010,  // was row 5
  B00000000,  // was row 4
  B00000000,  // was row 3
  B00100100,  // was row 2
  B00100100,  // was row 1
  B00000000   // was row 0
};

const byte hashtag[8] = {
  B00100100,
  B00100100,  //   #  #   
  B00100100,  //   #  #   
  B11111111,  // ########  horizontal bar 1
  B11111111,  // ########  horizontal bar 2
  B00100100,  //   #  #   
  B00100100,  //   #  #   
  B00100100
};


const byte digitColors[9][3] = {
  { 30,  90,  90},   // 1 - Ice Cyan
  { 40, 110,  40},   // 2 - Soft Green
  { 20,  70, 120},   // 3 - Cool Blue
  { 10, 130,  80},   // 4 - Mint
  {  0, 100, 140},   // 5 - Deep Cyan
  { 15,  60, 160},   // 6 - True Blue
  { 40,  50, 120},   // 7 - Indigo
  { 20, 140,  20},   // 8 - Neon Green
  { 80,  90, 100}    // 9 - Neutral White
};

LedControl lc(DIN, CLK, CS, UNITS);

void setup() {
  Serial.begin(9600);  // Add serial debugging
  
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(INCREMENT_BTN, INPUT_PULLUP);
  pinMode(RESET_BTN, INPUT_PULLUP);

  for (int i = 0; i < UNITS; i++) {
    lc.setIntensity(i, 1);
  }

  clearDisplay();
  showNumber();
  
  Serial.println("Setup complete!");
}

void playSound() {
  tone(BUZZER, 659);
  delay(150);
  tone(BUZZER, 784);
  delay(150);
  tone(BUZZER, 880);
  delay(150);
  tone(BUZZER, 1047);
  delay(400);
  noTone(BUZZER);
  delay(100);
  
  tone(BUZZER, 1047);
  delay(200);
  tone(BUZZER, 988);
  delay(200);
  tone(BUZZER, 880);
  delay(200);
  tone(BUZZER, 784);
  delay(400);
  noTone(BUZZER);
}

void clearDisplay() {
  for (int i = 0; i < UNITS; i++) {
    lc.shutdown(i, false);
    lc.clearDisplay(i);
  }
}

void showNumber() {
  for (int r = 0; r < 8; r++) {
    lc.setRow(0, r, hashtag[r]);
    lc.setRow(1, r, digits[count][r]);
    lc.setRow(2, r, digits[count][r]);
    lc.setRow(3, r, smileyFace[r]);
  }
}

void bounceNumber(int number) {
  for (int unit = UNITS - 1; unit >= 0; unit--) {
    clearDisplay();
    for (int r = 0; r < 8; r++) {
      lc.setRow(unit, r, digits[number][r]);
    }
    delay(200);
  }
  
  for (int unit = 1; unit < UNITS; unit++) {
    clearDisplay();
    for (int r = 0; r < 8; r++) {
      lc.setRow(unit, r, digits[number][r]);
    }
    delay(200);
  }
  
  clearDisplay();
  for (int r = 0; r < 8; r++) {
    lc.setRow(0, r, digits[number][r]);
  }
  delay(200);
}

void slideNumber(int number) {
  for (int scroll = 32; scroll >= 0; scroll--) {
    clearDisplay();
    
    int unit = scroll / 8;
    int shift = scroll % 8;
    
    if (unit < UNITS) {
      for (int row = 0; row < 8; row++) {
        byte digitByte = digits[number][row];
        
        if (shift > 0 && unit < UNITS) {
          lc.setRow(unit, row, digitByte >> shift);
        } else if (unit < UNITS) {
          lc.setRow(unit, row, digitByte);
        }
        
        if (shift > 0 && unit > 0) {
          lc.setRow(unit - 1, row, digitByte << (8 - shift));
        }
      }
    }
    delay(40);
  }
}

void zoomIn(int number) {
  for (int zoom = 0; zoom < 4; zoom++) {
    clearDisplay();
    
    for (int unit = 0; unit < UNITS; unit++) {
      for (int r = 0; r < 8; r++) {
        byte rowData = digits[number][r];
        
        if (zoom == 0) rowData = rowData & B00011000;
        else if (zoom == 1) rowData = rowData & B00111100;
        else if (zoom == 2) rowData = rowData & B01111110;
        
        lc.setRow(unit, r, rowData);
      }
    }
    delay(150);
  }
  
  clearDisplay();
  showNumber();
  delay(200);
}

void clearLED() {
  analogWrite(RED, 0);
  analogWrite(GREEN, 0);
  analogWrite(BLUE, 0);
}

void emitColorOnLED() {
  if (count == 0) {
    clearLED();
  } else {
    analogWrite(RED, digitColors[count - 1][0]);
    analogWrite(GREEN, digitColors[count - 1][1]);
    analogWrite(BLUE, digitColors[count - 1][2]);
  }
}

void loop() {
  
  // RESET - just check if button is pressed
  if (digitalRead(RESET_BTN) == LOW && count != 0) {
    count--;
    emitColorOnLED();
    showNumber();
    playSound();
    delay(1000);  // Simple delay to avoid repeat
  }
  
  // INCREMENT - check if button is pressed
  if (digitalRead(INCREMENT_BTN) == LOW) {
    count++;
    if (count > 9) count = 0;
    
    clearDisplay();
    emitColorOnLED();

    playSound();
    bounceNumber(count);
    delay(300);
    slideNumber(count);
    delay(300);
    zoomIn(count);
    delay(300);
    
    clearDisplay();
    showNumber();
    
    // Wait for button release
    while (digitalRead(INCREMENT_BTN) == LOW) {
      delay(10);
    }
    delay(50);  // Extra delay after release
  }
}
