#include "Arduino.h"
#include <task.h>
#include <queue.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>
#include "MAX30105.h"
#include "heartRate.h"


// MAX30105 particleSensor;

// void setup()
// {
//   Serial.begin(115200);
//   // Initialize sensor
//   if (particleSensor.begin() == false)
//   {
//     Serial.println("MAX30105 was not found. Please check wiring/power. ");
//     while (1)
//       ;
//   }

//   particleSensor.setup(); // Configure sensor. Use 6.4mA for LED drive
// }

// void loop()
// {
//   uint32_t Red = particleSensor.getRed();
//   uint32_t IR = particleSensor.getIR();
//   uint32_t Green = particleSensor.getGreen();
//   int points;
//   if (IR <= 10000)
//   {
//     points = map(IR, 420, 10000, 5, 6);
//   }
//   else if (IR > 10000 && IR <= 50000)
//   {
//     points = map(IR, 10000, 50000, 6, 8);
//   }
//   else if (IR > 50000 && IR <= 100000)
//   {
//     points = map(IR, 50000, 100000, 8, 10);
//   }
//   else if (IR > 100000 && IR <= 102000)
//   {
//     points = map(IR, 100000, 102000, 10, 12);
//   }
//   else if (IR > 102000 && IR <= 104000)
//   {
//     points = map(IR, 102000, 104000, 12, 15);
//   }
//   else if (IR > 104000 && IR <= 106000)
//   {
//     points = map(IR, 104000, 106000, 15, 20);
//   }
//   else if (IR > 106000 && IR <= 108000)
//   {
//     points = map(IR, 106000, 108000, 25, 30);
//   }
//   else if (IR > 108000 && IR <= 110000)
//   {
//     points = map(IR, 108000, 110000, 35, 40);
//   }
//   else
//   {
//     points = map(IR, 110000, 120000, 40, 55); // Add this else to handle values above 110000
//   }

//   // Serial.print(" R[");
//   // Serial.print(particleSensor.getRed());
//   // Serial.print("] IR[");
//   // Serial.print(particleSensor.getIR());
//   // Serial.print("] G[");
//   // Serial.print(particleSensor.getGreen());
//   // Serial.print("]");
//   Serial.printf("%d,%d,%d,%d\n", Red, IR, Green, points);
// }




/*
  Optical Heart Rate Detection (PBA Algorithm) using the MAX30105 Breakout
  By: Nathan Seidle @ SparkFun Electronics
  Date: October 2nd, 2016
  https://github.com/sparkfun/MAX30105_Breakout

  This is a demo to show the reading of heart rate or beats per minute (BPM) using
  a Penpheral Beat Amplitude (PBA) algorithm.

  It is best to attach the sensor to your finger using a rubber band or other tightening
  device. Humans are generally bad at applying constant pressure to a thing. When you
  press your finger against the sensor it varies enough to cause the blood in your
  finger to flow differently which causes the sensor readings to go wonky.

  Hardware Connections (Breakoutboard to Arduino):
  -5V = 5V (3.3V is allowed)
  -GND = GND
  -SDA = A4 (or SDA)
  -SCL = A5 (or SCL)
  -INT = Not connected

  The MAX30105 Breakout can handle 5V or 3.3V I2C logic. We recommend powering the board with 5V
  but it will also run at 3.3V.
*/

MAX30105 particleSensor;

const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred

float beatsPerMinute;
int beatAvg;

void setup()
{
  Serial.begin(115200);
  Serial.println("Initializing...");

  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");

  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED
}

void loop()
{
  long irValue = particleSensor.getIR();

  if (checkForBeat(irValue) == true)
  {
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }

  Serial.print("IR=");
  Serial.print(irValue);
  Serial.print(", BPM=");
  Serial.print(beatsPerMinute);
  Serial.print(", Avg BPM=");
  Serial.print(beatAvg);

  if (irValue < 50000)
    Serial.print(" No finger?");

  Serial.println();
}

