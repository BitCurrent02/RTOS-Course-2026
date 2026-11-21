#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

constexpr uint32_t TASK_DELAY_MS = 20;

QueueHandle_t packageQueue = nullptr;
TaskHandle_t packageTaskHandle = nullptr;
hw_timer_t *packageTimer = nullptr;

void ARDUINO_ISR_ATTR onPackage() {
    static uint32_t sequenceNumber = 0;
    BaseType_t taskWoken = pdFALSE;

    ++sequenceNumber;
    xQueueSendFromISR(packageQueue, &sequenceNumber, &taskWoken);

    if (taskWoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

void packageTask(void *) {
    uint32_t receivedNumber;

    while (true) {
        if (xQueueReceive(packageQueue, &receivedNumber, portMAX_DELAY) == pdTRUE) {
            Serial0.println(receivedNumber);
            vTaskDelay(pdMS_TO_TICKS(TASK_DELAY_MS));
        }
    }
}

void setup() {
    Serial0.begin(115200);

    packageQueue = xQueueCreate(4, sizeof(uint32_t));

    if (packageQueue == nullptr) {
        Serial0.println("Queue creation failed.");
        return;
    }

    packageTimer = timerBegin(1000000);
    if (packageTimer == nullptr) {
        Serial0.println("Timer creation failed.");
        return;
    }

    if (xTaskCreate(packageTask, "Packages", 4096, nullptr, 2,
                    &packageTaskHandle) != pdPASS) {
        Serial0.println("Task creation failed.");
        timerEnd(packageTimer);
        return;
    }

    timerAttachInterrupt(packageTimer, onPackage);
    timerAlarm(packageTimer, 100000, true, 0);  // One interrupt every 100 ms.
}

void loop() {
    delay(1000);
}