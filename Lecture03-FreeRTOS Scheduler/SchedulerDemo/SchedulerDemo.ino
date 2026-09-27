TaskHandle_t taskBHandle = NULL;

void taskA(void *p)
{
  const char *msg = "Task A is printing slowly...\n";

  for (;;)
  {
    for (size_t i = 0; msg[i] != '\0'; i++)
    {
      if (i == 12)
      {
        xTaskNotifyGive(taskBHandle);
      }

      Serial.print(msg[i]);
    }

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}


void taskB(void *p)
{
  for (;;)
  {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    Serial.print("*");
  }
}


void setup()
{
  Serial.begin(115200);

  xTaskCreatePinnedToCore(
    taskB,
    "Task B",
    2048,
    NULL,
    2,
    &taskBHandle,
    1
  );

  xTaskCreatePinnedToCore(
    taskA,
    "Task A",
    2048,
    NULL,
    1,
    NULL,
    1
  );
}


void loop()
{
  vTaskDelay(pdMS_TO_TICKS(1000));
}