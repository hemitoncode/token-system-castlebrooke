#include <LedControl.h>

const int DIN = 11;
const int CLK = 13;
const int CS  = 10;
const int UNITS = 4;  
const int INCREMENT_BTN = 2;

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

LedControl lc(DIN, CLK, CS, UNITS);

void setup() {
  pinMode(INCREMENT_BTN, INPUT_PULLUP);
  clearDisplay();
}

void clearDisplay() {
  for (int i = 0; i < UNITS; i++) {
    lc.shutdown(i, false);
    lc.clearDisplay(i);
  }
}

void showNumber() {
  for (int r = 0; r < 8; r++) {
    lc.setRow(0, r, digits[count][r]);
    lc.setRow(1, r, digits[count][r]);
    lc.setRow(2, r, digits[count][r]);
    lc.setRow(3, r, digits[count][r]);
  }
}

// Flash on all units simultaneously
void flashAll(int number, int times) {
  for (int i = 0; i < times; i++) {
    // Show on all units
    for (int unit = 0; unit < UNITS; unit++) {
      for (int r = 0; r < 8; r++) {
        lc.setRow(unit, r, digits[number][r]);
      }
    }
    delay(150);
    
    // Clear all
    clearDisplay();
    delay(150);
  }
}

// Bounce back and forth between units
void bounceNumber(int number) {
  // Bounce right to left
  for (int unit = UNITS - 1; unit >= 0; unit--) {
    clearDisplay();
    for (int r = 0; r < 8; r++) {
      lc.setRow(unit, r, digits[number][r]);
    }
    delay(200);
  }
  
  // Bounce left to right
  for (int unit = 1; unit < UNITS; unit++) {
    clearDisplay();
    for (int r = 0; r < 8; r++) {
      lc.setRow(unit, r, digits[number][r]);
    }
    delay(200);
  }
  
  // Back to unit 0
  clearDisplay();
  for (int r = 0; r < 8; r++) {
    lc.setRow(0, r, digits[number][r]);
  }
  delay(200);
}

// Slide smoothly from right to left
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

// Zoom in effect - number grows from center
void zoomIn(int number) {
  // Start small in center
  for (int zoom = 0; zoom < 4; zoom++) {
    clearDisplay();
    
    for (int unit = 0; unit < UNITS; unit++) {
      for (int r = 0; r < 8; r++) {
        byte rowData = digits[number][r];
        
        if (zoom == 0) rowData = rowData & B00011000;  // Tiny
        else if (zoom == 1) rowData = rowData & B00111100;  // Small
        else if (zoom == 2) rowData = rowData & B01111110;  // Medium
        
        lc.setRow(unit, r, rowData);
      }
    }
    delay(150);
  }
  
  // Final full size on unit 0
  clearDisplay();
  showNumber();
  delay(200);
}

void loop() {
  if (digitalRead(INCREMENT_BTN) == HIGH) {
    count++;
    if (count > 9) count = 1;
    
    clearDisplay();
    
    // Choose animation sequence
    flashAll(count, 2);      // Flash 2 times
    delay(300);
    
    bounceNumber(count);     // Bounce back and forth
    delay(300);
    
    slideNumber(count);      // Slide to position
    delay(300);
    
    zoomIn(count);           // Zoom in effect
    delay(300);
        
    // Final display
    clearDisplay();
    showNumber();
    
    while (digitalRead(INCREMENT_BTN) == HIGH);
  }
}
