/* My changes are in line 79 and 339
  Lecture 12: Multicore Systems

  ESP32-S3 Dual-Core Producer-Consumer-Monitor Demonstration
  
  Board: ESP32-S3 DevKitC-1
  Serial Monitor: 115200 baud

  Architecture:

  Producer Task - Core 0
       |
       | FreeRTOS Queue
       v
  Consumer Task - Core 1
       |
       | writes
       v
  Shared Statistics
       ^
       | reads
       |
  Monitor - Arduino loop()

  Consumer and Monitor use the SAME mutex
  when accessing Shared Statistics.
*/

#include <Arduino.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"


// ------------------------------------------------------------
// Data transferred through the queue
// ------------------------------------------------------------

struct SensorReading {
  uint32_t sequence;
  float value;
  TickType_t producedAt;
};


// ------------------------------------------------------------
// Shared data
// ------------------------------------------------------------

struct SharedStatistics {
  uint32_t sampleCount;
  float sum;
  float runningAverage;
  float minimum;
  float maximum;
};


// ------------------------------------------------------------
// Configuration
// ------------------------------------------------------------

constexpr UBaseType_t QUEUE_LENGTH = 8;

constexpr uint32_t PRODUCER_PERIOD_MS = 500;

constexpr uint32_t QUEUE_WAIT_MS = 100;

constexpr uint32_t MUTEX_WAIT_MS = 50;

constexpr uint32_t MONITOR_PERIOD_MS = 3000;

constexpr uint32_t PRODUCER_STACK = 4096;

constexpr uint32_t CONSUMER_STACK = 4096;

//change from me
constexpr float TEMPERATURE_THRESHOLD = 24.0f;

// ------------------------------------------------------------
// FreeRTOS objects
// ------------------------------------------------------------

QueueHandle_t readingQueue = nullptr;

SemaphoreHandle_t statisticsMutex = nullptr;


// ------------------------------------------------------------
// Shared statistics
// ------------------------------------------------------------

SharedStatistics statistics = {
  0,      // sampleCount
  0.0f,   // sum
  0.0f,   // runningAverage
  0.0f,   // minimum
  0.0f    // maximum
};


// ------------------------------------------------------------
// Fatal error handler
// ------------------------------------------------------------

void fatalError(const char *message) {

  Serial.printf("FATAL: %s\n", message);

  Serial.flush();

  while (true) {
    delay(1000);
  }
}


// ------------------------------------------------------------
// Simulated sensor
// ------------------------------------------------------------

float makeSimulatedReading(uint32_t sequence) {

  // Temperature-like signal:
  //
  // 22.0
  // 22.4
  // 22.8
  // ...
  // 24.8
  // then repeat

  return 22.0f +
         static_cast<float>(sequence % 8U) * 0.4f;
}


// ============================================================
// TASK 1: SENSOR PRODUCER
// Core 0
// ============================================================

void sensorProducerTask(void *parameter) {

  (void)parameter;

  uint32_t sequence = 0;

  TickType_t lastWake = xTaskGetTickCount();


  for (;;) {

    SensorReading reading;

    reading.sequence = sequence++;

    reading.value =
        makeSimulatedReading(reading.sequence);

    reading.producedAt =
        xTaskGetTickCount();


    // Send the complete reading to the queue.
    //
    // The producer does NOT access shared statistics.
    // Communication with the consumer happens through the queue.

    if (xQueueSend(
            readingQueue,
            &reading,
            pdMS_TO_TICKS(QUEUE_WAIT_MS)) == pdPASS) {

      Serial.printf(
          "[Producer | Core %d] "
          "seq=%lu value=%.2f queued=%u\n",

          xPortGetCoreID(),

          static_cast<unsigned long>(
              reading.sequence),

          reading.value,

          static_cast<unsigned>(
              uxQueueMessagesWaiting(readingQueue)));

    } else {

      Serial.printf(
          "[Producer | Core %d] "
          "queue full, dropped seq=%lu\n",

          xPortGetCoreID(),

          static_cast<unsigned long>(
              reading.sequence));
    }


    // Produce one reading every 500 ms.

    vTaskDelayUntil(
        &lastWake,
        pdMS_TO_TICKS(PRODUCER_PERIOD_MS));
  }
}


// ============================================================
// TASK 2: DATA CONSUMER
// Core 1
// ============================================================

void dataConsumerTask(void *parameter) {

  (void)parameter;

  SensorReading reading;


  for (;;) {

    // Wait for data.
    //
    // When the queue is empty, this task blocks.
    // It does not continuously poll the queue.

    if (xQueueReceive(
            readingQueue,
            &reading,
            portMAX_DELAY) == pdPASS) {


      SharedStatistics snapshot;

      bool updated = false;


      // ------------------------------------------------------
      // Critical shared-data access
      // ------------------------------------------------------
      //
      // Consumer WRITES statistics.
      //
      // The Monitor in loop() also READS statistics.
      //
      // Therefore both must use the SAME mutex.

      if (xSemaphoreTake(
              statisticsMutex,
              pdMS_TO_TICKS(MUTEX_WAIT_MS)) == pdTRUE) {


        // First sample initializes min and max.

        if (statistics.sampleCount == 0) {

          statistics.minimum =
              reading.value;

          statistics.maximum =
              reading.value;

        } else {

          if (reading.value <
              statistics.minimum) {

            statistics.minimum =
                reading.value;
          }

          if (reading.value >
              statistics.maximum) {

            statistics.maximum =
                reading.value;
          }
        }


        // Update statistics.

        statistics.sampleCount++;

        statistics.sum +=
            reading.value;

        statistics.runningAverage =
            statistics.sum /
            statistics.sampleCount;


        // Copy a consistent snapshot while
        // the mutex is still held.

        snapshot = statistics;

        updated = true;


        // Release as soon as possible.

        xSemaphoreGive(
            statisticsMutex);
      }


      // ------------------------------------------------------
      // Slow operations happen AFTER releasing the mutex.
      // ------------------------------------------------------

      if (updated) {

        Serial.printf(
            "[Consumer | Core %d] "
            "seq=%lu avg=%.2f "
            "min=%.2f max=%.2f "
            "count=%lu\n",

            xPortGetCoreID(),

            static_cast<unsigned long>(
                reading.sequence),

            snapshot.runningAverage,

            snapshot.minimum,

            snapshot.maximum,

            static_cast<unsigned long>(
                snapshot.sampleCount));

      // my changes: Threshold alarm
      if (reading.value > TEMPERATURE_THRESHOLD) {
          Serial.println("ALARM: Temperature too high!");
      }

      } else {

        Serial.printf(
            "[Consumer | Core %d] "
            "mutex timeout for seq=%lu\n",

            xPortGetCoreID(),

            static_cast<unsigned long>(
                reading.sequence));
      }
    }
  }
}


// ============================================================
// SETUP
// ============================================================

void setup() {

  Serial.begin(115200);

  delay(1000);


  Serial.println();

  Serial.println(
      "Lecture 12 - "
      "ESP32-S3 Dual-Core Data Processing with Threshold Alarm");


  Serial.printf(
      "setup() is running on core %d\n",
      xPortGetCoreID());


  // ----------------------------------------------------------
  // Create queue
  // ----------------------------------------------------------

  readingQueue =
      xQueueCreate(
          QUEUE_LENGTH,
          sizeof(SensorReading));


  if (readingQueue == nullptr) {

    fatalError(
        "Could not create reading queue");
  }


  // ----------------------------------------------------------
  // Create mutex
  // ----------------------------------------------------------

  statisticsMutex =
      xSemaphoreCreateMutex();


  if (statisticsMutex == nullptr) {

    vQueueDelete(readingQueue);

    readingQueue = nullptr;

    fatalError(
        "Could not create statistics mutex");
  }


  // ----------------------------------------------------------
  // Create Producer Task
  //
  // Explicitly pinned to Core 0.
  // ----------------------------------------------------------

  BaseType_t producerCreated =
      xTaskCreatePinnedToCore(

          sensorProducerTask,

          "SensorProducer",

          PRODUCER_STACK,

          nullptr,

          2,

          nullptr,

          0);


  if (producerCreated != pdPASS) {

    fatalError(
        "Could not create producer task on Core 0");
  }


  // ----------------------------------------------------------
  // Create Consumer Task
  //
  // Explicitly pinned to Core 1.
  // ----------------------------------------------------------

  BaseType_t consumerCreated =
      xTaskCreatePinnedToCore(

          dataConsumerTask,

          "DataConsumer",

          CONSUMER_STACK,

          nullptr,

          2,

          nullptr,

          1);


  if (consumerCreated != pdPASS) {

    fatalError(
        "Could not create consumer task on Core 1");
  }


  Serial.println(
      "Tasks created. Watch the core IDs below.");
}


// ============================================================
// MONITOR
//
// Arduino loop() acts as a simple monitoring task.
//
// Consumer:
//    WRITES SharedStatistics
//
// Monitor:
//    READS SharedStatistics
//
// Both use statisticsMutex.
// ============================================================

void loop() {

  static uint32_t lastMonitorTime = 0;


  if (millis() - lastMonitorTime >=
      MONITOR_PERIOD_MS) {

    lastMonitorTime = millis();


    SharedStatistics snapshot;

    bool copied = false;


    // --------------------------------------------------------
    // Take mutex before reading shared data.
    // --------------------------------------------------------

    if (xSemaphoreTake(
            statisticsMutex,
            pdMS_TO_TICKS(MUTEX_WAIT_MS)) == pdTRUE) {


      // Copy the WHOLE structure while protected.

      snapshot = statistics;

      copied = true;


      // Immediately release the mutex.

      xSemaphoreGive(
          statisticsMutex);
    }


    // --------------------------------------------------------
    // Print AFTER releasing the mutex.
    // --------------------------------------------------------

    if (copied) {

      Serial.printf(
          "[Monitor  | Core %d] "
          "count=%lu avg=%.2f "
          "min=%.2f max=%.2f\n",

          xPortGetCoreID(),

          static_cast<unsigned long>(
              snapshot.sampleCount),

          snapshot.runningAverage,

          snapshot.minimum,

          snapshot.maximum);

    } else {

      Serial.printf(
          "[Monitor  | Core %d] "
          "mutex timeout\n",

          xPortGetCoreID());
    }
  }


  // loop() should not busy-spin.

  delay(10);
}