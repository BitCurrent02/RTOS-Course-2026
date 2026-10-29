#include <Arduino.h>

QueueHandle_t temperatureQueue;


// --------------------------------------------------
// Sensor Task
// --------------------------------------------------

void sensorTask(void *parameter)
{
  int temperature = 20;

  while (1)
  {
    // TODO 1:
    // Send the temperature to the queue
    xQueueSend(temperatureQueue, &temperature, 0);

    // TODO 2:
    // Increase the temperature by 1
    temperature++;

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}


// --------------------------------------------------
// Display Task
// --------------------------------------------------

void displayTask(void *parameter)
{
  int receivedTemperature;

  while (1)
  {
    // TODO 3:
    // Receive a temperature value from the queue
    if (xQueueReceive(temperatureQueue, &receivedTemperature, 0) == pdPASS)
    {
      
      // TODO 4:
      // If a value was received, print:
      // Temperature: XX C
      Serial.printf("Temperature: %d C°\n", receivedTemperature);
    }

    vTaskDelay(pdMS_TO_TICKS(100));
  }
}


// --------------------------------------------------
// Setup
// --------------------------------------------------

void setup()
{
  Serial.begin(115200);

  // TODO 5:
  // Create a queue that can store 5 integers
  temperatureQueue = xQueueCreate(5, sizeof(int));

  if (temperatureQueue == NULL)
  {
    Serial.println("Queue creation failed");
    return;
  }

  // TODO 6:
  // Create the Sensor Task

  xTaskCreate(
    sensorTask,
    "Sensor",
    4096,
    NULL,
    1,
    NULL
  );


  // TODO 7:
  // Create the Display Task

  xTaskCreate(
    displayTask,
    "Display",
    4096,
    NULL,
    1,
    NULL
  );

}


void loop()
{
}