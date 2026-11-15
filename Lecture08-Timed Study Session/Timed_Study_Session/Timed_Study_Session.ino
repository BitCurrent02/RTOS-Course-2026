#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Hardware: ESP32-S3-DevKitC-1 v1.1
// CHANGE these two pins to match your previous OLED exercise.
static const int OLED_SDA = 8;
static const int OLED_SCL = 9;
static const int RGB_LED_PIN = 38;

static const int SCREEN_WIDTH = 128;
static const int SCREEN_HEIGHT = 64;
static const uint8_t OLED_ADDRESS = 0x3C;

static const uint32_t UPDATE_INTERVAL_MS = 1000;
static const uint32_t SESSION_DURATION_MS = 10000;
static const uint32_t EVENT_UPDATE = (1UL << 0);
static const uint32_t EVENT_COMPLETE = (1UL << 1);

#if CONFIG_FREERTOS_UNICORE
static const BaseType_t APP_CPU = 0;
#else
static const BaseType_t APP_CPU = 1;
#endif

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
TimerHandle_t updateTimer = nullptr;
TimerHandle_t completeTimer = nullptr;
TaskHandle_t applicationTaskHandle = nullptr;

// Only the application task reads/writes these variables.
bool sessionRunning = false;
uint32_t sessionStartMs = 0;

void setStudyLED(bool on) {
  // This board uses an addressable RGB LED, not a regular digital LED.
  neopixelWrite(RGB_LED_PIN, 0, on ? 32 : 0, 0); // Green / Off
}

void showScreen(const char *firstLine, const char *secondLine = nullptr) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 12);
  display.println(firstLine);
  if (secondLine != nullptr) {
    display.setCursor(0, 30);
    display.println(secondLine);
  }
  display.display();
}

void showRemainingTime() {
  uint32_t elapsedMs = (uint32_t)(millis() - sessionStartMs);
  uint32_t remainingMs =
      elapsedMs >= SESSION_DURATION_MS ? 0 : SESSION_DURATION_MS - elapsedMs;
  uint32_t remainingSeconds = (remainingMs + 999) / 1000;

  char text[32];
  snprintf(text, sizeof(text), "Remaining: %lu s",
           (unsigned long)remainingSeconds);
  showScreen("Session running", text);
}

// FreeRTOS software timer callbacks run in the timer service task.
// They ONLY send notifications; no OLED, Serial or blocking work.
void onUpdateTimer(TimerHandle_t timer) {
  (void)timer;
  if (applicationTaskHandle != nullptr) {
    xTaskNotify(applicationTaskHandle, EVENT_UPDATE, eSetBits);
  }
}

void onCompleteTimer(TimerHandle_t timer) {
  (void)timer;
  if (applicationTaskHandle != nullptr) {
    xTaskNotify(applicationTaskHandle, EVENT_COMPLETE, eSetBits);
  }
}

void startSession() {
  if (sessionRunning) return; // Ignore extra 's' commands.

  // Timers were created but remain dormant until a session starts.
  // Check both timer-command results before updating the interface.
  if (xTimerStart(updateTimer, pdMS_TO_TICKS(20)) != pdPASS) {
    Serial.println("ERROR: Could not start update timer.");
    return;
  }

  if (xTimerStart(completeTimer, pdMS_TO_TICKS(20)) != pdPASS) {
    Serial.println("ERROR: Could not start completion timer.");
    if (xTimerStop(updateTimer, pdMS_TO_TICKS(20)) != pdPASS) {
      Serial.println("ERROR: Could not stop update timer.");
    }
    return;
  }

  sessionStartMs = millis();
  sessionRunning = true;
  setStudyLED(true);
  showScreen("Session running", "Remaining: 10 s");
  Serial.println("Session started.");
}

void finishSession() {
  if (!sessionRunning) return;

  sessionRunning = false;
  if (xTimerStop(updateTimer, pdMS_TO_TICKS(20)) != pdPASS) {
    Serial.println("ERROR: Could not stop update timer.");
  }

  setStudyLED(false);
  showScreen("Session complete");
  Serial.println("Session complete.");
}

void applicationTask(void *parameter) {
  (void)parameter;

  for (;;) {
    // Continuously inspect Serial, without an indefinite notification wait.
    while (Serial.available() > 0) {
      char command = (char)Serial.read();
      if (command == 's' && !sessionRunning) {
        startSession();
      }
      // Ignore other characters, line endings, and extra 's' commands.
    }

    uint32_t events = 0;
    if (xTaskNotifyWait(0, EVENT_UPDATE | EVENT_COMPLETE, &events,
                        pdMS_TO_TICKS(20)) == pdTRUE) {
      // Completion wins if both notifications arrive together.
      if ((events & EVENT_COMPLETE) && sessionRunning) {
        finishSession();
      } else if ((events & EVENT_UPDATE) && sessionRunning) {
        showRemainingTime();
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  setStudyLED(false);

  Wire.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS, true, false)) {
    Serial.println("ERROR: SSD1306 initialization failed.");
    while (true) vTaskDelay(pdMS_TO_TICKS(1000));
  }

  showScreen("Ready", "Send s to start");
  Serial.println("Ready. Send s to start.");

  updateTimer = xTimerCreate("UpdateTimer", pdMS_TO_TICKS(UPDATE_INTERVAL_MS),
                             pdTRUE, nullptr, onUpdateTimer);
  completeTimer = xTimerCreate("CompleteTimer", pdMS_TO_TICKS(SESSION_DURATION_MS),
                               pdFALSE, nullptr, onCompleteTimer);

  if (updateTimer == nullptr || completeTimer == nullptr) {
    Serial.println("ERROR: Timer creation failed.");
    showScreen("Timer error");
    while (true) vTaskDelay(pdMS_TO_TICKS(1000));
  }

  BaseType_t created = xTaskCreatePinnedToCore(
      applicationTask, "StudyTask", 6144, nullptr, 1,
      &applicationTaskHandle, APP_CPU);
  if (created != pdPASS) {
    Serial.println("ERROR: Application task creation failed.");
    showScreen("Task error");
    while (true) vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void loop() {
  // All study-session work is owned by the FreeRTOS application task.
  vTaskDelay(pdMS_TO_TICKS(1000));
}