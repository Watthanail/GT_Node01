// #include <Arduino.h>
// #include <SPI.h>
// #include <Wire.h>
// #include <OLED_SSD1306_Chart.h>
// #include <Adafruit_I2CDevice.h> //Include this to avoid compile errors in Platformio

// void setup() {
//   Wire.begin();
//   Serial.begin(115200);
//   Serial.println("\nI2C Scanner");
// }
 
// void loop() {
//   byte error, address;
//   int nDevices;
//   Serial.println("Scanning...");
//   nDevices = 0;
//   for(address = 1; address < 127; address++ ) {
//     Wire.beginTransmission(address);
//     error = Wire.endTransmission();
//     if (error == 0) {
//       Serial.print("I2C device found at address 0x");
//       if (address<16) {
//         Serial.print("0");
//       }
//       Serial.println(address,HEX);
//       nDevices++;
//     }
//     else if (error==4) {
//       Serial.print("Unknow error at address 0x");
//       if (address<16) {
//         Serial.print("0");
//       }
//       Serial.println(address,HEX);
//     }    
//   }
//   if (nDevices == 0) {
//     Serial.println("No I2C devices found\n");
//   }
//   else {
//     Serial.println("done\n");
//   }
//   delay(5000);          
// }

/*
  Name:    SinglePlotMode.ino
  Created:  04/01/2020 11:59:52 AM
  Author: José Gabriel Companioni Benítez (https://github.com/elC0mpa)
  Description: Example to demonstrate how to draw cartesian charts in Adafruit SSD1306 oleds in single plot mode 
*/

// #include <Arduino.h>
// #include <SPI.h>
// #include <Wire.h>
// #include <OLED_SSD1306_Chart.h>
// #include <Adafruit_I2CDevice.h> //Include this to avoid compile errors in Platformio

// #define SCREEN_WIDTH 128 // OLED display width, in pixels
// #define SCREEN_HEIGHT 64 // OLED display height, in pixels

// // Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// #define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)



// OLED_SSD1306_Chart display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// char actualThickness;

// void setup()
// {
// // // put your setup code here, to run once:
// // #if defined ESP8266
// //   Wire.begin(SDA_PIN, SCL_PIN);
// // #else
// //   Wire.begin();
// // #endif

//   display.begin(SSD1306_SWITCHCAPVCC, 0x3c);
//   display.clearDisplay();
//   display.setChartCoordinates(0, 60);      //Chart lower left coordinates (X, Y)
//   display.setChartWidthAndHeight(123, 55); //Chart width = 123 and height = 60
//   display.setXIncrement(5);                //Distance between Y points will be 5px
//   display.setYLimits(50, 100);             //Ymin = 0 and Ymax = 100
//   display.setYLimitLabels("50", "100");
//   display.setYLabelsVisible(true);
//   display.setAxisDivisionsInc(12, 6);    //Each 12 px a division will be painted in X axis and each 6px in Y axis
//   display.setPlotMode(SINGLE_PLOT_MODE); //Set single plot mode
//   // display.setPointGeometry(POINT_GEOMETRY_CIRCLE);
//   actualThickness = NORMAL_LINE;
//   display.setLineThickness(actualThickness);
//   display.drawChart(); //Update the buffer to draw the cartesian chart
//   display.display();
// }

// void loop()
// {
//   // put your main code here, to run repeatedly:
//   auto value = random(50) + 50;
//   if (!display.updateChart(value)) //Value between Ymin and Ymax will be added to chart
//   {
//     display.clearDisplay(); //If chart is full, it is drawn again
//     if (actualThickness == NORMAL_LINE)
//     {
//       actualThickness = LIGHT_LINE;
//     }
//     else if (actualThickness == LIGHT_LINE)
//     {
//       actualThickness = NORMAL_LINE;
//     }
//     display.setLineThickness(actualThickness);
//     display.drawChart();
//   }
//   delay(100);
// }

// #include <Wire.h>
// #include <Adafruit_GFX.h>
// // #include <Adafruit_SSD1306.h>
// #include <Adafruit_SH1106.h>

// #define SCREEN_WIDTH 128
// #define SCREEN_HEIGHT 32

// #define LOGO_WIDTH 16
// #define LOGO_HEIGHT 16

// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// static const unsigned char PROGMEM heart[] =
// { B00000000, B00000000,
//   B00000000, B00000000,
//   B00111100, B00111100,
//   B01111110, B11111110,
//   B11111111, B11111111,
//   B11111111, B11111111,
//   B01111111, B11111110,
//   B00111111, B11111100,
//   B00011111, B11111000,
//   B00001111, B11110000,
//   B00000111, B11100000,
//   B00000011, B11000000,
//   B00000001, B10000000,
//   B00000000, B00000000,
//   B00000000, B00000000,
//   B00000000, B00000000};

// void setup() {
//   display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
//   display.display();
//   delay(2000); // Pause for 2 seconds

//   display.clearDisplay();
//   display.setTextSize(2);      // Normal 1:1 pixel scale
//   display.setTextColor(WHITE); // Draw white text
//   display.cp437(true);         // Use full 256 char 'Code Page 437' font
  
//   display.println(F(" Health Kids "));
  
//   display.drawLine(0, 28, 43, 28, WHITE);
//   display.drawLine(71, 28, 120, 28, WHITE);
  
//   display.drawBitmap(
//     50,
//     20,
//     heart, LOGO_WIDTH, LOGO_HEIGHT, 1);
  
//   display.display();
// }

// void updateDisplay(float value) {
//   display.clearDisplay();
//   display.setTextSize(2);      // Normal 1:1 pixel scale
//   display.setTextColor(WHITE); // Draw white text
//   display.setCursor(0, 0);     // Start at top-left corner
//   display.cp437(true);         // Use full 256 char 'Code Page 437' font
  
//   display.println(F(" Dev 2024"));
  
//   display.drawLine(0, 28, 43, 28, WHITE);
//   display.drawLine(71, 28, 120, 28, WHITE);
  
//   display.drawBitmap(
//     50,
//     20,
//     heart, LOGO_WIDTH, LOGO_HEIGHT, 1);
  
//   display.setCursor(0, 40);     // Start at top-left corner
//   display.print(F("Batt: "));
//   display.print(value, 1); // Print value with 1 decimal place
  
//   display.display();
// }

// void loop() {
//   // Put your main code here, to run repeatedly:
//   float value = 3.0 + (random(1, 5) / 10.0); // Generate a float value between 3.1 and 3.4
//   updateDisplay(value);
//   delay(1000);
// }

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>

#define OLED_RESET 4
Adafruit_SH1106 display(OLED_RESET);

#define LOGO_WIDTH 16
#define LOGO_HEIGHT 16
#define MAX_POINTS 128 // Width of the OLED display

int points[MAX_POINTS];
int currentIndex = 0;
int chartWidth = 128;
int chartHeight = 64;

void setup() {
  display.begin(SH1106_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(2000); // Pause for 2 seconds
}

void drawAxisDivisions1() {
  // Draw y divisions
  for (int i = 0; i <= chartHeight; i += 5) {
    display.drawFastHLine(20 - 3, i, 3, WHITE);
  }

  // Draw x divisions
  for (int i = 20; i <= chartWidth; i += 5) {
    display.drawFastVLine(i, 30, 3, WHITE);
    display.drawFastVLine(i, 60, 3, WHITE);
  }

}




void drawChart2() {
  display.clearDisplay();
  display.setTextSize(0.5);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(110, 5);
  display.println(F("EKG"));

  display.setTextSize(0.5);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(110, 35);
  display.println(F("BMP"));

  display.setTextSize(0.1);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(10, 25);
  display.println(F("0"));
  display.setTextSize(0.1);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(2, 0);
  display.println(F("100"));

  display.setTextSize(0.1);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(10, 55);
  display.println(F("0"));
  display.setTextSize(0.1);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(2, 35);
  display.println(F("100"));
  
  // Draw x and y axis 1
  display.drawLine(20, 64, 20, 0, WHITE); // y-axis
  display.drawLine(20, 30, chartWidth, 30, WHITE); // x-axis
  display.drawLine(20, 60, chartWidth, 60, WHITE); // x-axis

  // Draw axis divisions
  drawAxisDivisions1();

  // Draw x and y labels
  // display.setCursor(0, chartHeight + 4); // Set cursor for x label
  // display.print("X Label");
  
  // display.setCursor(-24, chartHeight / 2); // Set cursor for y label
  // display.print("Y Label");
  
  // Draw the chart
  // for (int i = 0; i < MAX_POINTS - 1; i++) {
  //   display.drawLine(i, chartHeight - points[i], i + 1, chartHeight - points[i + 1], WHITE);
  // }
  
  display.display();
}



void loop() {
  // Put your main code here, to run repeatedly:
  // int value = random(0, chartHeight); // Generate a random value between 0 and chartHeight
  // points[currentIndex] = value;
  // currentIndex++;
  
  // if (currentIndex >= MAX_POINTS) {
  //   // Clear display plot after reaching the end of MAX_POINTS
  //   for (int i = 0; i < MAX_POINTS; i++) {
  //     points[i] = 0;
  //   }
  //   currentIndex = 0;
  //   display.clearDisplay(); // Clear the display after resetting the plot
  // }
  
  drawChart2();
  
  delay(100); // Delay for visibility
}

