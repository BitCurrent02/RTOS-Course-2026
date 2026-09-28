#define RGB_BUILTIN 38

volatile uint32_t blinkInterval = 500;

TaskHandle_t serialTaskHandle = NULL;
TaskHandle_t ledTaskHandle = NULL;

// Task A - Serial Task
void taskA(void *parameter)
{
  while (true)
  {
    if (Serial.available())
    {
      String input = Serial.readStringUntil('\n');
      input.trim();

      if (input == "suspend")
      {
        vTaskSuspend(ledTaskHandle);
        Serial.println("LED task suspended");
      }

      else if (input == "resume")
      {
        vTaskResume(ledTaskHandle);
        Serial.println("LED task resumed");
      }

      else
      {
        int value = input.toInt();

        if (value == 250 || value == 500 || value == 1000)
        {
          blinkInterval = value;
          Serial.println("New blink interval");
        }

        else
        {
          Serial.println("Invalid input");
        }
      }
    }

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}


// Task B - RGB LED Task
void taskB(void *parameter)
{
  while (true)
  {
    neopixelWrite(RGB_BUILTIN, 50, 0, 0);
    vTaskDelay(pdMS_TO_TICKS(blinkInterval));

    neopixelWrite(RGB_BUILTIN, 0, 0, 0);
    vTaskDelay(pdMS_TO_TICKS(blinkInterval));
  }
}


void setup()
{
  Serial.begin(115200);

  xTaskCreatePinnedToCore(
    taskA,
    "Task A",
    2048,
    NULL,
    1,
    &serialTaskHandle,
    1
  );

  xTaskCreatePinnedToCore(
    taskB,
    "Task B",
    2048,
    NULL,
    1,
    &ledTaskHandle,
    1
  );
}


void loop()
{
}