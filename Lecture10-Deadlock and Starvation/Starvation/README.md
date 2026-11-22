# Assignment 2: Starvation

## Part A
### Which tasks may experience starvation? Explain why.
There are two tasks that never get to run (starvation). The Task_Display and the Task_Log have both lower priorities than Task_Control and while Task_Control is without delay only this task is running.
### Why is it dangerous for a high-priority task to run continuously without blocking?
Because Task that have a lower priority never run. it is like a new offset für the lowest priority.
### Give two ways to allow lower-priority tasks to get CPU time without changing task priorities.
To block them with vTaskDelay(). Or let the higher priority task wait for something. 

## Part B

### Step 1 – Observe starvation

#### Output
```
=== Demo 2: Starvation | STARVATION_FIX 0 ===
t= 499 ms | Task_Hog: SLEEP | Task_Low progress: +500
t= 999 ms | Task_Hog: SLEEP | Task_Low progress: +500
t=1499 ms | Task_Hog: SLEEP | Task_Low progress: +500
t=1999 ms | Task_Hog: SLEEP | Task_Low progress: +500
t=2499 ms | Task_Hog: SLEEP | Task_Low progress: +500
t=2999 ms | Task_Hog: SLEEP | Task_Low progress: +500
t=3499 ms | Task_Hog: BUSY  | Task_Low progress: +0
t=3999 ms | Task_Hog: BUSY  | Task_Low progress: +0
t=4499 ms | Task_Hog: BUSY  | Task_Low progress: +0
t=4999 ms | Task_Hog: BUSY  | Task_Low progress: +0
t=5499 ms | Task_Hog: BUSY  | Task_Low progress: +0
t=5999 ms | Task_Hog: SLEEP | Task_Low progress: +1
t=6499 ms | Task_Hog: SLEEP | Task_Low progress: +500
t=6999 ms | Task_Hog: SLEEP | Task_Low progress: +500
t=7499 ms | Task_Hog: SLEEP | Task_Low progress: +500
t=7999 ms | Task_Hog: SLEEP | Task_Low progress: +500
t=8499 ms | Task_Hog: SLEEP | Task_Low progress: +500
t=8999 ms | Task_Hog: BUSY  | Task_Low progress: +499
t=9499 ms | Task_Hog: BUSY  | Task_Low progress: +0
t=9999 ms | Task_Hog: BUSY  | Task_Low progress: +0
t=10499 ms | Task_Hog: BUSY  | Task_Low progress: +0
t=10999 ms | Task_Hog: BUSY  | Task_Low progress: +0
t=11499 ms | Task_Hog: BUSY  | Task_Low progress: +0
```
#### What is the progress of Task_Low while Task_Hog is BUSY?
The Task_Low progress is always +0. That is because Task_Hog has a higher priority.
#### What is the progress of Task_Low while Task_Hog is SLEEP?
In that case Task_Low makes +500 progress every 500 ms.
#### What do these results tell you about starvation?
The results show that a high-priority task can prevent lower-priority tasks from running if it never blocks.

### Step 2 – Enable the fix

#### Output
```
=== Demo 2: Starvation | STARVATION_FIX 1 ===
t= 499 ms | Task_Hog: BUSY  | Task_Low progress: +50
t= 999 ms | Task_Hog: BUSY  | Task_Low progress: +50
t=1499 ms | Task_Hog: BUSY  | Task_Low progress: +50
t=1999 ms | Task_Hog: BUSY  | Task_Low progress: +50
t=2499 ms | Task_Hog: BUSY  | Task_Low progress: +50
t=2999 ms | Task_Hog: SLEEP | Task_Low progress: +59
t=3499 ms | Task_Hog: SLEEP | Task_Low progress: +500
t=3999 ms | Task_Hog: SLEEP | Task_Low progress: +500
t=4499 ms | Task_Hog: SLEEP | Task_Low progress: +500
t=4999 ms | Task_Hog: SLEEP | Task_Low progress: +500
t=5499 ms | Task_Hog: SLEEP | Task_Low progress: +500
t=5999 ms | Task_Hog: BUSY  | Task_Low progress: +491
t=6499 ms | Task_Hog: BUSY  | Task_Low progress: +50
t=6999 ms | Task_Hog: BUSY  | Task_Low progress: +50
t=7499 ms | Task_Hog: BUSY  | Task_Low progress: +50
t=7999 ms | Task_Hog: BUSY  | Task_Low progress: +50
t=8499 ms | Task_Hog: BUSY  | Task_Low progress: +50
t=8999 ms | Task_Hog: SLEEP | Task_Low progress: +59
t=9499 ms | Task_Hog: SLEEP | Task_Low progress: +500
t=9999 ms | Task_Hog: SLEEP | Task_Low progress: +500
```
#### Comparison 
Now is the Task_Low progress always +50 while the Task_Hog is Busy. And while the Task_Hog is sleeping the full +500.
#### Explain why Task_Low can now make progress even while Task_Hog is busy.
Because now the higher priority Task_Hog has a vTaskDelay(1) on a regulary basis. In the short time it is Blocked allows the Task_Low to run occasionally.

### Step 3 – Change the delay
#### Output
```
=== Demo 2: Starvation | STARVATION_FIX 1 ===
t= 499 ms | Task_Hog: BUSY  | Task_Low progress: +266
t= 999 ms | Task_Hog: BUSY  | Task_Low progress: +264
t=1499 ms | Task_Hog: BUSY  | Task_Low progress: +260
t=1999 ms | Task_Hog: BUSY  | Task_Low progress: +265
t=2499 ms | Task_Hog: BUSY  | Task_Low progress: +265
t=2999 ms | Task_Hog: SLEEP | Task_Low progress: +267
t=3499 ms | Task_Hog: SLEEP | Task_Low progress: +500
t=3999 ms | Task_Hog: SLEEP | Task_Low progress: +500
t=4499 ms | Task_Hog: SLEEP | Task_Low progress: +500
t=4999 ms | Task_Hog: SLEEP | Task_Low progress: +500
t=5499 ms | Task_Hog: SLEEP | Task_Low progress: +500
t=5999 ms | Task_Hog: BUSY  | Task_Low progress: +493
t=6499 ms | Task_Hog: BUSY  | Task_Low progress: +264
t=6999 ms | Task_Hog: BUSY  | Task_Low progress: +266
t=7499 ms | Task_Hog: BUSY  | Task_Low progress: +260
t=7999 ms | Task_Hog: BUSY  | Task_Low progress: +263
t=8499 ms | Task_Hog: BUSY  | Task_Low progress: +266
t=8999 ms | Task_Hog: SLEEP | Task_Low progress: +266
t=9499 ms | Task_Hog: SLEEP | Task_Low progress: +500
t=9999 ms | Task_Hog: SLEEP | Task_Low progress: +500
```
#### Prediction
I think nothing will change because now the Blocked state is longer, but the Low task isn't working the whole time, and because of that, nothing changes.

#### Observation
My prediction was wrong. Task_Low made much more progress during the BUSY period. With vTaskDelay(1), the progress was about +50, but with vTaskDelay(10), it increased to about +260 to +266. This happens probably because Task_Hog stays in the Blocked state longer, giving Task_Low more CPU time and it is also working the whole time.