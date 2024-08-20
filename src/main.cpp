#include "Arduino.h"
#include <task.h>
#include <queue.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>
#include "MAX30105.h"
#include "heartRate.h"

// WIFI
#include <WiFi.h>
#include <WiFiClient.h>

// MQTT
#include <PubSubClient.h>
// config mqtt/wifi
#define CFG_WIFI_SSID "nopawan_2.4G"  //"AIS_2.4G"
#define CFG_WIFI_PASS "kai0859445852" //"nice2meetu"
#define CFG_MQTT_SERVER "103.253.73.68"
#define CFG_MQTT_PORT 1883
#define CFG_MQTT_USER "taistdev"
#define CFG_MQTT_PASS "taistdev"
// #define MQTT_HB_TOPIC "taistdev/node01" // public_heartbeat
#define MQTT_Data_TOPIC "taist2024/node01/data"

WiFiClient wifi_client;
PubSubClient mqtt_client(wifi_client);

MAX30105 BMPSensor;

#define OLED_RESET 4
Adafruit_SH1106 display(OLED_RESET);

#define LOGO_WIDTH 16
#define LOGO_HEIGHT 16
#define MAX_POINTS 128 // Width of the OLED display
int IRpoints[MAX_POINTS];
int EKGpoints[MAX_POINTS];
int chartWidth = 128;
int chartHeight = 64;
int currentIndex = 0;
// long irValue;
// Queue handle
QueueHandle_t evt_queue1;
QueueHandle_t EKGQueue;
QueueHandle_t irValueQueue;
QueueHandle_t dataQueue;
TaskHandle_t batteryTaskHandle = NULL;
TaskHandle_t graphTaskHandle = NULL;
TaskHandle_t MenuTaskHandle = NULL;

// Sensor Data
float beatAvg;
float beatsPerMinute;
float batt;

// bool buttonState;

struct DataPacket
{
  bool buttonState;
  long irValue;
  uint32_t EKGValue;
};

DataPacket data;
// Display states enumeration

static const unsigned char PROGMEM heart[] = {
    B00000000, B00000000,
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
    B00000000, B00000000};

void resetIRPoints()
{
  for (int i = 0; i < MAX_POINTS; i++)
  {
    IRpoints[i] = 0;
  }
}

void resetEKGPoints()
{
  for (int i = 0; i < MAX_POINTS; i++)
  {
    EKGpoints[i] = 0;
  }
}

void heartrate_task(void *pvParameter)
{
  Serial.begin(115200);
  Serial.println("Initializing...");

  // Initialize heart rate sensor
  // Initialize heart rate sensor
  if (!BMPSensor.begin(Wire, I2C_SPEED_FAST, 0x57))
  {
    Serial.println("MAX30105 was not found. Please check wiring/power.");
    while (1)
      ;
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");
  bool lastButtonState = LOW;
  bool reading;
  unsigned long lastDebounceTime = 0;
  const unsigned long debounceDelay = 50; // Debounce delay in milliseconds

  // Setup for leads off detection
  pinMode(D0, INPUT);
  pinMode(D1, INPUT);
  pinMode(D6, INPUT);
  BMPSensor.setup();                    // Configure sensor with default settings
  BMPSensor.setPulseAmplitudeRed(0x0A); // Turn Red LED to low to indicate sensor is running
  BMPSensor.setPulseAmplitudeGreen(0);  // Turn off Green LED

  const byte RATE_SIZE = 4; // Increase this for more averaging. 4 is good.
  byte rates[RATE_SIZE];    // Array of heart rates
  byte rateSpot = 0;
  long lastBeat = 0; // Time at which the last beat occurred

  uint32_t previousTime = 0;

  while (true)
  {
    data.irValue = BMPSensor.getIR();
    reading = digitalRead(D6);
    data.EKGValue = analogRead(A2);
    Serial.println(data.EKGValue);

    if (checkForBeat(data.irValue) == true)
    {
      // We sensed a beat!
      long delta = millis() - lastBeat;
      lastBeat = millis();

      beatsPerMinute = 60 / (delta / 1000.0);

      if (beatsPerMinute < 255 && beatsPerMinute > 20)
      {
        rates[rateSpot++] = (byte)beatsPerMinute; // Store this reading in the array
        rateSpot %= RATE_SIZE;                    // Wrap variable

        // Take average of readings
        beatAvg = 0;
        for (byte x = 0; x < RATE_SIZE; x++)
          beatAvg += rates[x];
        beatAvg /= RATE_SIZE;
      }
    }

    if (reading != lastButtonState)
    {
      lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay)
    {
      if (reading != data.buttonState)
      {
        data.buttonState = reading;
      }
    }
    if (data.irValue > 80000 || data.buttonState == HIGH)
    {
      xQueueSend(dataQueue, &data, 0);
    }

    lastButtonState = reading;

    if ((millis() - previousTime) >= 10000) // Check if 10 seconds have passed since button press
    {
      vTaskResume(batteryTaskHandle);
      previousTime = millis();
    }
  }
}

void battery_task(void *pvParameter)
{
  pinMode(A3, INPUT);
  uint32_t Vbatt = 0;
  while (true)
  {
    Vbatt = 0; // Reset Vbatt for each set of readings
    for (int i = 0; i < 16; i++)
    {
      Vbatt += analogReadMilliVolts(A3); // ADC with correction
    }
    batt = 1.95 * Vbatt / 16 / 1000.0; // Convert to voltage
    vTaskSuspend(NULL);
  }
}

void drawAxisDivisions1()
{
  // Draw y divisions
  for (int i = 0; i <= chartHeight; i += 5)
  {
    display.drawFastHLine(20 - 3, i, 3, WHITE);
  }

  // Draw x divisions
  for (int i = 20; i <= chartWidth; i += 5)
  {
    display.drawFastVLine(i, 30, 3, WHITE);
    display.drawFastVLine(i, 60, 3, WHITE);
  }
}

void drawChart2()
{

  display.clearDisplay();
  // display.setTextSize(0.5);    // Normal 1:1 pixel scale
  // display.setTextColor(WHITE); // Draw white text
  // display.setCursor(110, 5);
  // display.println(F("EKG"));

  // display.setTextSize(0.5);    // Normal 1:1 pixel scale
  // display.setTextColor(WHITE); // Draw white text
  // display.setCursor(90, 5);
  // display.println(data.buttonState);
  display.setTextSize(0.1);    // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(2, 0);
  display.println(F("EKG"));

  display.setTextSize(0.5);    // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(100, 35);
  display.println(beatAvg);

  // display.setTextSize(0.1);    // Normal 1:1 pixel scale
  // display.setTextColor(WHITE); // Draw white text
  // display.setCursor(10, 25);
  // display.println(F("0"));
  display.setTextSize(0.1);    // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(2, 40);
  display.println(F("BMP"));

  // display.setTextSize(0.1);    // Normal 1:1 pixel scale
  // display.setTextColor(WHITE); // Draw white text
  // display.setCursor(2, 40);
  // display.println(beatAvg);

  // Draw x and y axis 1
  display.drawLine(20, 64, 20, 0, WHITE);          // y-axis
  display.drawLine(20, 30, chartWidth, 30, WHITE); // x-axis
  display.drawLine(20, 60, chartWidth, 60, WHITE); // x-axis

  // Draw axis divisions
  drawAxisDivisions1();

  // Draw the chart
  for (int i = 20; i < MAX_POINTS - 1; i++)
  {
    if (data.buttonState == HIGH)
    {
      int EKG_y1 = map(EKGpoints[i], 0, 4095, 30, 10); // Map the points to display height
      int EKG_y2 = map(EKGpoints[i + 1], 0, 4095, 30, 10);
      display.drawLine(i, EKG_y1, i + 1, EKG_y2, WHITE);
    }

    int IR_y1 = map(IRpoints[i], 5, 80, 59, 40);
    int IR_y2 = map(IRpoints[i + 1], 5, 80, 59, 40);
    display.drawLine(i, IR_y1, i + 1, IR_y2, WHITE);
  }

  display.display();
}

void displayTaskMenu(void *pvParameters)
{
  display.begin(SH1106_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  while (true)
  {
    display.clearDisplay(); // Clear the display before updating it

    // Update static display elements for MENU state
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.cp437(true);
    display.println(F(" Dev 2024"));

    display.drawLine(0, 28, 43, 28, WHITE);
    display.drawLine(71, 28, 120, 28, WHITE);

    display.drawBitmap(50, 20, heart, LOGO_WIDTH, LOGO_HEIGHT, 1);

    display.setCursor(0, 40);
    display.print(F("Batt: "));
    display.print(batt, 1); // Assuming batt is a global variable

    // Update display for GRAPH state
    // if (xQueueReceive(irValueQueue, &irValue, 0) || xQueueReceive(EKGQueue, &buttonState, 0) == pdPASS)
    if (xQueueReceive(dataQueue, &data, 0) == pdPASS)
    // if (xQueueReceive(irValueQueue, &data.irValue, 0) == pdPASS)
    {
      IRpoints[currentIndex] = map(data.irValue, 108000, 120000, 20, 80);
      EKGpoints[currentIndex] = data.EKGValue;
      currentIndex++;
      if (currentIndex >= MAX_POINTS)
      {
        for (int i = 20; i < MAX_POINTS - 1; i++)
        {
          if (data.buttonState == HIGH)
          {
            EKGpoints[i] = EKGpoints[i + 1];
          }
          IRpoints[i] = IRpoints[i + 1];
        }

        currentIndex = MAX_POINTS - 1;
      }

      if (data.irValue <= 100000)
      {
        // IRpoints[MAX_POINTS - 1] = map(data.irValue, 50000, 108000, 5, 5);
        beatAvg = 0;
        beatsPerMinute = 0;
        resetIRPoints();
      }
      drawChart2(); // Update the chart on the display
    }
    // resetEKGPoints();

    display.display(); // Display the updated content
  }
}

void comm_task1(void *pvParameter)
{
  // float value;

  // while (true)
  // {
  //   if (xQueueReceive(evt_queue1, &value, portMAX_DELAY) == pdPASS)
  //   {
  //     Serial.print("BPM=");
  //     Serial.print(value);
  //     Serial.print(" Battery=");
  //     Serial.print(batt);
  //     Serial.println();
  //   }
  // }
  // initialize serial and network
  Serial.begin(115200);

  WiFi.begin(CFG_WIFI_SSID, CFG_WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  mqtt_client.setServer(CFG_MQTT_SERVER, CFG_MQTT_PORT);
  mqtt_client.connect("taist_watthanai", CFG_MQTT_USER, CFG_MQTT_PASS);
  while (true)
  {
    char payload[500];
    if (xQueueReceive(dataQueue, &data, 0) == pdPASS)
    // if (xQueueReceive(irValueQueue, &data.irValue, 0) == pdPASS)
    {
      snprintf(payload, sizeof(payload), "{\"device_addr\": \"dev01\", \"value\": {\"IRvalue\": %.2f, \"EKG01\": %.2f, \"Battery\": %.2f}}", data.irValue, data.EKGValue, batt);
      mqtt_client.publish("MQTT_Data_TOPIC", payload);
      
    }
  }
}

void setup()
{
  evt_queue1 = xQueueCreate(10, sizeof(float));
  EKGQueue = xQueueCreate(10, sizeof(bool));
  irValueQueue = xQueueCreate(10, sizeof(long));
  dataQueue = xQueueCreate(10, sizeof(DataPacket));

  xTaskCreatePinnedToCore(
      heartrate_task, // Task function
      "HeartRate",    // Name of task
      2048,           // Stack size of task
      NULL,           // Parameter of the task
      4,              // Priority of the task
      NULL,           // Task handle
      0);

  xTaskCreatePinnedToCore(
      battery_task,       // Task function
      "Battery",          // Name of task
      2048,               // Stack size of task
      NULL,               // Parameter of the task
      4,                  // Priority of the task
      &batteryTaskHandle, // Task handle to keep track of created task
      0                   // Core to run the task on (0 or 1)
  );

  xTaskCreatePinnedToCore(
      displayTaskMenu, // Task function
      "oled",          // Name of task
      2048,            // Stack size of task
      NULL,            // Parameter of the task
      3,               // Priority of the task
      NULL,            // Task handle
      0);

  xTaskCreatePinnedToCore(
      comm_task1, // Task function
      "Comm1",    // Name of task
      2048,       // Stack size of task
      NULL,       // Parameter of the task
      2,          // Priority of the task
      NULL,       // Task handle
      0);
}

// void loop()
// {
// }

void loop()
{
  // execute MQTT loop
  if (mqtt_client.connected())
  {
    mqtt_client.loop();
  }
  delay(1000);
}