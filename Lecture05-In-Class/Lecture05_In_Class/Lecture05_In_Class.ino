#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

// NEW: DHT22
#include <DHT.h>

// NEW: DHT22
#define DHTPIN 4
#define DHTTYPE DHT22

// NEW: DHT22
DHT dht(DHTPIN, DHTTYPE);

// Define the queue
QueueHandle_t myQueue = NULL;

// Change these values for the experiments
const int queueSize = 5;
const int producerMs = 1000;
const int consumerMs = 1000;


// Producer task
void producerTask(void *parameter)
{
  int value = 1;

  while (1)
  {
    // Try to send. Do not wait for space
    if (xQueueSend(myQueue, &value, 0) != pdPASS)
    {
      Serial.println("Queue full: value not sent");
    }

    // Increase the number even if sending fails
    value++;

    vTaskDelay(pdMS_TO_TICKS(producerMs));
  }
}


// Consumer task
void consumerTask(void *parameter)
{
  int value;

  while (1)
  {
    // Receive one item. Do not wait for data
    if (xQueueReceive(myQueue, &value, 0) == pdPASS)
    {
      Serial.printf("Received: %d\n", value);
    }

    vTaskDelay(pdMS_TO_TICKS(consumerMs));
  }
}


// NEW: DHT22 task
void dhtTask(void *parameter)
{
  while (1)
  {
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    if (!isnan(temperature) && !isnan(humidity))
    {
      Serial.print("Temperature: ");
      Serial.print(temperature);
      Serial.println(" °C");

      Serial.print("Humidity: ");
      Serial.print(humidity);
      Serial.println(" %");
    }
    else
    {
      Serial.println("DHT22 reading failed");
    }

    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}


void setup()
{
  Serial.begin(115200);
  delay(1000);

  // NEW: DHT22
  dht.begin();

  // Create a queue that can hold five integers
  myQueue = xQueueCreate(queueSize, sizeof(int));

  if (myQueue == NULL)
  {
    Serial.println("Queue creation failed");
    return;
  }

  // Both tasks use the same priority
  BaseType_t producerResult = xTaskCreate(
    producerTask,
    "Producer",
    2048,
    NULL,
    1,
    NULL
  );

  BaseType_t consumerResult = xTaskCreate(
    consumerTask,
    "Consumer",
    2048,
    NULL,
    1,
    NULL
  );

  // NEW: DHT22
  BaseType_t dhtResult = xTaskCreate(
    dhtTask,
    "DHT22",
    2048,
    NULL,
    1,
    NULL
  );

  if (producerResult != pdPASS ||
      consumerResult != pdPASS ||
      dhtResult != pdPASS)
  {
    Serial.println("Task creation failed");
  }
}


void loop()
{
  // The producer, consumer and sensor tasks do the work
  delay(1000);
}