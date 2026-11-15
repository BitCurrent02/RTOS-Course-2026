# Timed Study Session

## Why does this system need both a one-shot and an auto-reload timer?
The application task need the one-shot timer to send a single notification to end the session. But the Application task need to update the countdown the auto reload timer to send a notification.
## Which task executes the timer callbacks?
The Booth run in the Timer service task.
## Why do OLED updates belong in the application task?
because it takes more time and only the callbacks send a signal that they can finish quickely.
## When are the timers dormant?
the timer are dormant befor it starts. After 10 Seconds the one shot timer stops because its a oneshot timer and the application task stops the auto reload timer
## How does your program prevent extra s commands from extending the session?
Because the session ignores extra s. Thats aslong as the session runs. 