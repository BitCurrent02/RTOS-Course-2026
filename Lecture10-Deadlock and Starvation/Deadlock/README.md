# Assignment 1: Deadlock

## Part A

### Which two tasks can cause a deadlock? Explain how they can end up waiting for each other.
The two tasks Task_Sensor and Task_Logger can cause a deadlock. That happens when the sensor task holds the i2cMutex and the Task_Logger holds the uartMutex. Both of them are also waiting for the other mutex and are stuck.
### Can Task_Display create a circular wait by itself? Explain why or why not.
I would say no because the Task_Display only holds the i2cMutex once. That means it can wait for that one but doesn't hold any other mutex that any of the other tasks needs.
### Which Coffman condition is removed when all tasks acquire mutexes in the same order?
Using the same lock order prevents circular wait. A task waiting for the first mutex does not yet hold the second one.

## Part B

### Step 1

#### Output
```
=== Demo 1: Deadlock | DEMO_MODE 0 ===
[Task_1] taking A ...
[Task_1] has A, now needs B
[Task_2] taking B ...
[Task_2] has B, now needs A
[HEARTBEAT] t=2 s | rounds: Task_1=0 Task_2=0
[HEARTBEAT] t=3 s | rounds: Task_1=0 Task_2=0
[HEARTBEAT] t=4 s | rounds: Task_1=0 Task_2=0
```
#### What happens to the worker tasks?
In that case both of them are blocked. As we can see the Task_1 holds Mutex A and needs Mutex B. And Task_2 holds Mutex B and needs Mutex A. Each are waiting for the other Mutex.
#### Which numbers in the HEARTBEAT line stop changing?
As we can see both rounds counter didn't change at all. But the t is still increasing.

### Step 2

#### Output
```
=== Demo 1: Deadlock | DEMO_MODE 0 ===
[Task_1] taking A ...
[Task_1] has A, now needs B
[Task_2] taking A ...
[Task_1] has BOTH locks -> work done (round 1)
[Task_2] has A, now needs B
[Task_1] taking A ...
[Task_2] has BOTH locks -> work done (round 1)
```
#### Explanation
It is working now because both tasks now take mutex A as first mutex and after that utex B. As we can see now while Task_2 waits for mutex A, it holds no mutex at all. Then Task_1 can also take mutex B and finishing its work and after that also releasing both mutexes again. Task_2 can then do the same.

### Step 3
I also undid the changes from the last step otherwise there was no timeout.
#### What happens when a task cannot acquire the second mutex before the timeout?
It gives back the mutex and then tries again later.
#### Why must the task release its first mutex?
So that the other task can take it and finish its work and its round.
#### What is the purpose of the random backoff?
The purpose is that because of that random time it is less likely that both try again at the same time.