#include "Arduino.h"
#include <task.h>
#include <queue.h>
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"

MAX30105 particleSensor;
// queue handle
QueueHandle_t evt_queue1;
QueueHandle_t evt_queue2;
TaskHandle_t batteryTaskHandle = NULL;

////**** SensorData ****////

float beatAvg;
float batt;
bool buttonState;
uint32_t ecg;

////************************////
void heartrate_task(void *pvParameter)
{
    Serial.begin(115200);
    Serial.println("Initializing...");

    // Initialize heart rate sensor
    if (!particleSensor.begin(Wire, I2C_SPEED_FAST))
    {
        Serial.println("MAX30105 was not found. Please check wiring/power.");
        while (1)
            ;
    }
    Serial.println("Place your index finger on the sensor with steady pressure.");

    particleSensor.setup();                    // Configure sensor with default settings
    particleSensor.setPulseAmplitudeRed(0x0A); // Turn Red LED to low to indicate sensor is running
    particleSensor.setPulseAmplitudeGreen(0);  // Turn off Green LED

    const byte RATE_SIZE = 4; // Increase this for more averaging. 4 is good.
    byte rates[RATE_SIZE];    // Array of heart rates
    byte rateSpot = 0;
    long lastBeat = 0; // Time at which the last beat occurred
    float beatsPerMinute = 0;
    // float beatAvg = 0;
    uint32_t previousTime = 0;

    while (true)
    {
        long irValue = particleSensor.getIR();

        if (checkForBeat(irValue) == true)
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

        if (irValue > 50000)
        {

            // if (beatsPerMinute > 0)
            // {

                xQueueSend(evt_queue1, &beatAvg, portMAX_DELAY);
            // }
        }
        else
        {
            beatAvg = 0;
            beatsPerMinute = 0;
            // xQueueSend(evt_queue, &beatAvg, portMAX_DELAY);
            Serial.print(irValue);
            Serial.print(" No finger?");
            Serial.print(" ,Battery ");
            Serial.print(batt);
            Serial.println();

            if ((millis() - previousTime) >= 10000) // Check if 10 seconds have passed since button press
            {

                vTaskResume(batteryTaskHandle);
                previousTime = millis();
            }
        }
    }
}

void ECG_task(void *pvParameter)
{
    Serial.begin(115200);

    // Setup for leads off detection
    pinMode(D0, INPUT);
    pinMode(D1, INPUT);
    pinMode(D6, INPUT);

    while (true)
    {
        buttonState = digitalRead(D6);
        Serial.println(buttonState);
        if (buttonState == HIGH)
        {

            // if (digitalRead(D0) == HIGH || digitalRead(D1) == HIGH)
            // {
            //   Serial.println('!');
            // }
            // else
            // {

            xQueueSend(evt_queue2, &buttonState, portMAX_DELAY);
            // Serial.println(analogRead(A3));
            // Send the value of analog input A3
            // }
        }

        // Wait for a bit to keep serial data from saturating
        vTaskDelay(100);
    }
}

void battery_task(void *pvParameter)
{
    pinMode(A2, INPUT);
    uint32_t Vbatt = 0;
    while (true)
    {
        Vbatt = 0; // Reset Vbatt for each set of readings
        for (int i = 0; i < 16; i++)
        {
            Vbatt += analogReadMilliVolts(A2); // ADC with correction
        }
        batt = 1.95 * Vbatt / 16 / 1000.0; // Convert to voltage
        // Serial.printf(" ,Battery= %.2f",Data.batt);

        vTaskSuspend(NULL);
    }
}

void comm_task1(void *pvParameter)
{
    float value;

    while (true)
    {

        if (xQueueReceive(evt_queue1, &value, portMAX_DELAY) == pdPASS)
        {
            Serial.print("BPM=");
            Serial.print(value);
            Serial.print(" Battery=");
            Serial.print(batt);
            Serial.println();
        }


        // else if (queue1 == pdPASS && queue2 == pdPASS)
        // {
        //   Serial.print("BPM=");
        //   Serial.print(value1);
        //   Serial.print(" ECG=");
        //   Serial.print(value2);
        //   Serial.print(" Battery=");
        //   Serial.print(batt);
        //   Serial.println();
        // }
    }
}

void comm_task2(void *pvParameter)
{

    bool value;


    while (true)
    {

        if (xQueueReceive(evt_queue2, &value, portMAX_DELAY) == pdPASS)
        {
            Serial.print("EKG=");
            Serial.print(value);
            Serial.print(" Battery=");
            Serial.print(batt);
            Serial.println();
        }

        // else if (queue1 == pdPASS && queue2 == pdPASS)
        // {
        //   Serial.print("BPM=");
        //   Serial.print(value1);
        //   Serial.print(" ECG=");
        //   Serial.print(value2);
        //   Serial.print(" Battery=");
        //   Serial.print(batt);
        //   Serial.println();
        // }
    }
}

void setup()
{
    evt_queue1 = xQueueCreate(10, sizeof(float));
    evt_queue2 = xQueueCreate(10, sizeof(bool));
    // Initialize the serial communication
    // Serial.begin(115200);

    // // Setup for leads off detection
    // pinMode(D0, INPUT);
    // pinMode(D1, INPUT);
    // pinMode(D6, INPUT);
    xTaskCreatePinnedToCore(
        heartrate_task, // Task function
        "HeartRate",    // Name of task
        2048,           // Stack size of task
        NULL,           // Parameter of the task
        3,              // Priority of the task
        NULL,           // Task handle
        0);

    //   Set up the battery pin

    //   Create the battery task

    xTaskCreatePinnedToCore(
        battery_task,       // task function
        "Battery",          // name of task
        2048,               // stack size of task
        NULL,               // parameter of the task
        3,                  // priority of the task
        &batteryTaskHandle, // task handle to keep track of created task
        0                   // core to run the task on (0 or 1)
    );

    xTaskCreatePinnedToCore(
        comm_task1, // Task function
        "Comm1",    // Name of task
        2048,      // Stack size of task
        NULL,      // Parameter of the task
        2,         // Priority of the task
        NULL,      // Task handle
        0);

    xTaskCreatePinnedToCore(
        comm_task2, // Task function
        "Comm2",    // Name of task
        2048,      // Stack size of task
        NULL,      // Parameter of the task
        2,         // Priority of the task
        NULL,      // Task handle
        1);


    xTaskCreatePinnedToCore(
        ECG_task, // Task function
        "ecg",    // Name of task
        2048,     // Stack size of task
        NULL,     // Parameter of the task
        3,        // Priority of the task
        NULL,     // Task handle
        1);
}

void loop()
{
    // Check if leads are off
    // bool buttonState = digitalRead(D6);
    // if (buttonState != lastFlickerableState)
    // {
    //     lastDebounceTime = millis();
    //     lastFlickerableState = buttonState;
    // }
    // if ((millis() - lastDebounceTime) > Debouncetime)
    // {
    // Serial.print(buttonState);
    // else
    // {
    //     // Send the value of analog input A3

    //     //   if (buttonState == HIGH){
    //     //     xQueueSend(evt_queue2, &ecg, portMAX_DELAY);
    //     //     // Serial.println(analogRead(A3));

    //     //   }
    //     // Send the value of analog input A3
    // }
    // }

    // Wait for a bit to keep serial data from saturating
    delay(100);
}
