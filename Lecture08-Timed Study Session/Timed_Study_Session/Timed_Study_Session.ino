#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#if CONFIG_FREERTOS_UNICORE
static const BaseType_t APP_CPU = 0;
#else
static const BaseType_t APP_CPU = 1;
#endif

// Change these to match your previous OLED wiring.
const int SDA_PIN = 8;
const int SCL_PIN = 9;

// Used only if RGB_BUILTIN is not defined.
const int LED_PIN = 4;

static const uint32_t SESSION_MS = 10000;
static const uint32_t UPDATE_EVENT = 1UL << 0;
static const uint32_t COMPLETE_EVENT = 1UL << 1;

Adafruit_SSD1306 display(128, 64, &Wire, -1);

TaskHandle_t applicationTaskHandle = nullptr;
TimerHandle_t updateTimer = nullptr;
TimerHandle_t sessionTimer = nullptr;

void setLED(bool on) {
#ifdef RGB_BUILTIN
  rgbLedWrite(RGB_BUILTIN, 0, on ? 30 : 0, 0);
#else
  digitalWrite(LED_PIN, on ? HIGH : LOW);
#endif
}

void fail(const char *message) {
  setLED(false);
  Serial.println(message);

  while (true) {
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void showScreen(const char *line1, const char *line2 = "") {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.println(line1);

  display.setCursor(0, 20);
  display.println(line2);

  display.display();
}

void showRemaining(uint32_t seconds) {
  char text[24];
  snprintf(text, sizeof(text), "Remaining: %lu s",
           (unsigned long)seconds);

  showScreen("Session running", text);
}

void updateCallback(TimerHandle_t timer) {
  // Only notify the application task.
  xTaskNotify(applicationTaskHandle, UPDATE_EVENT, eSetBits);
}

void sessionCallback(TimerHandle_t timer) {
  // Only notify the application task.
  xTaskNotify(applicationTaskHandle, COMPLETE_EVENT, eSetBits);
}

void applicationTask(void *parameter) {
  bool sessionRunning = false;
  uint32_t startTime = 0;

  for (;;) {
    // Read Serial without waiting for input.
    while (Serial.available() > 0) {
      char character = (char)Serial.read();

      // Ignore all other characters and extra 's' commands.
      if (character == 's' && !sessionRunning) {
        uint32_t oldEvents;

        // Clear any pending events from the previous session.
        xTaskNotifyWait(
          0, UINT32_MAX, &oldEvents, 0
        );

        startTime = millis();
        sessionRunning = true;

        if (xTimerStart(updateTimer, pdMS_TO_TICKS(100))
            != pdPASS) {
          fail("Update timer start failed");
        }

        if (xTimerStart(sessionTimer, pdMS_TO_TICKS(100))
            != pdPASS) {
          if (xTimerStop(updateTimer, pdMS_TO_TICKS(100))
              != pdPASS) {
            fail("Session start and update stop failed");
          }
          fail("Session timer start failed");
        }

        setLED(true);
        showRemaining(10);
        Serial.println("Session started");
      }
    }

    uint32_t events = 0;

    // Wait briefly so Serial input is checked regularly.
    if (xTaskNotifyWait(
          0,
          UINT32_MAX,
          &events,
          pdMS_TO_TICKS(10)
        ) == pdTRUE) {

      // Completion has priority over a countdown update.
      if ((events & COMPLETE_EVENT) && sessionRunning) {
        if (xTimerStop(updateTimer, pdMS_TO_TICKS(100))
            != pdPASS) {
          fail("Update timer stop failed");
        }

        sessionRunning = false;
        setLED(false);

        showScreen("Session complete");
        Serial.println("Session complete");
      }
      else if ((events & UPDATE_EVENT) && sessionRunning) {
        uint32_t elapsed = millis() - startTime;

        uint32_t remainingMs =
          elapsed < SESSION_MS ? SESSION_MS - elapsed : 0;

        // Round up to whole seconds.
        uint32_t remainingSeconds =
          (remainingMs + 999) / 1000;

        showRemaining(remainingSeconds);
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1500);

#ifndef RGB_BUILTIN
  pinMode(LED_PIN, OUTPUT);
#endif
  setLED(false);

  Wire.begin(SDA_PIN, SCL_PIN);

  // false prevents the library from calling Wire.begin()
  // again after we have selected our SDA and SCL pins.
  if (!display.begin(
        SSD1306_SWITCHCAPVCC, 0x3C, true, false)) {
    fail("OLED initialization failed");
  }

  showScreen("Ready", "Send s to start");

  updateTimer = xTimerCreate(
    "Update",
    pdMS_TO_TICKS(1000),
    pdTRUE,
    nullptr,
    updateCallback
  );

  if (updateTimer == nullptr) {
    fail("Update timer creation failed");
  }

  sessionTimer = xTimerCreate(
    "Session",
    pdMS_TO_TICKS(SESSION_MS),
    pdFALSE,
    nullptr,
    sessionCallback
  );

  if (sessionTimer == nullptr) {
    fail("Session timer creation failed");
  }

  // Both timers remain dormant until 's' is received.
  if (xTaskCreatePinnedToCore(
        applicationTask,
        "Application",
        4096,
        nullptr,
        1,
        &applicationTaskHandle,
        APP_CPU
      ) != pdPASS) {
    fail("Application task creation failed");
  }

  Serial.println("Timers created: DORMANT.");
  Serial.println("Send s to start a 10-second session.");
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}