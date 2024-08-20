
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>

#define OLED_RESET 4
Adafruit_SH1106 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2


#define LOGO_WIDTH 16
#define LOGO_HEIGHT 16


static const unsigned char PROGMEM heart[] =
{ B00000000, B00000000,
  B00000000, B00000000,
  B00111100, B00111100,
  B01111110, B11111110,
  B11111111, B11111111,
  B11111111, B11111111,
  B01111111, B11111110,
  B00111111, B11111100,
  B00011111, B11111000,
  B00001111, B11110000,
  B00000111, B11100000,
  B00000011, B11000000,
  B00000001, B10000000,
  B00000000, B00000000,
  B00000000, B00000000,
  B00000000, B00000000 };

void setup() {
  display.begin(SH1106_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(2000); // Pause for 2 seconds

  display.clearDisplay();
  display.setTextSize(2);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  
  display.println(F(" Health Kids "));
  
  display.drawLine(0, 28, 43, 28, WHITE);
  display.drawLine(71, 28, 120, 28, WHITE);
  
  display.drawBitmap(
    50,
    20,
    heart, LOGO_WIDTH, LOGO_HEIGHT, 1);
  
  display.display();
}

void updateDisplay(float value) {
  display.clearDisplay();
  display.setTextSize(2);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  
  display.println(F(" Dev 2024"));
  
  display.drawLine(0, 28, 43, 28, WHITE);
  display.drawLine(71, 28, 120, 28, WHITE);
  
  display.drawBitmap(
    50,
    20,
    heart, LOGO_WIDTH, LOGO_HEIGHT, 1);
  
  display.setCursor(0, 40);     // Start at top-left corner
  display.print(F("Batt: "));
  display.print(value, 1); // Print value with 1 decimal place
  
  display.display();
}

void loop() {
  // Put your main code here, to run repeatedly:
  float value = 3.0 + (random(1, 5) / 10.0); // Generate a float value between 3.1 and 3.4
  updateDisplay(value);
  delay(1000);
}

