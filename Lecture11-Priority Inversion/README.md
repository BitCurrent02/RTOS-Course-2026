# Assignment 1: Priority Inversion

## Part A

### With a binary semaphore, what happens after Task_H requests the lock? Explain how Task_M affects Task_L.
Task_L has Lock -> task_H waits -> Task_M interrupts Task_L -> Task_H has to wait longer
### Draw a simple timeline showing which task runs in the binary semaphore configuration.
- 0-100 ms      Task_L runs 
- 100 ms        Task_H blocked
- 100-150 ms    Task_L runs 
- 150-1150 ms   Task_M runs
- 1150-1300 ms  Task_L runs and finishes
- 1300 ms  -    Task_H runs
### What changes when a mutex with priority inheritance is used? Draw or describe the new timeline.
- 0-300 ms      Task_L runs
- After 300 ms  Task_H runs
- After Task_H  Task_M runs
### Which task temporarily inherits a higher priority? What priority does it inherit?
Task_L temporarily inherits priority 3 from Task_H. After releasing the mutex, Task_L returns to its original priority of 1.

## Part B

### Step 1
 
#### Output
```
=== Demo 3: Priority Inversion | USE_MUTEX 0 (binary semaphore, NO inheritance) ===
[    0 ms] L: taking lock
[  100 ms] L: critical section 1/3, my priority = 1
[  101 ms] H: needs the lock
[  150 ms] M: starts 1000 ms of work (does not use the lock)
[ 1150 ms] M: finished
[ 1190 ms] L: critical section 2/3, my priority = 1
[ 1290 ms] L: critical section 3/3, my priority = 1
[ 1290 ms] H: got the lock after waiting 1189 ms
[ 1340 ms] H: done
```

#### The waiting time reported by Task_H.
1189 ms
#### The priority printed by Task_L.
1
#### The order in which Task_L, Task_M and Task_H complete their work.
Task_M -> Task_L -> Task_H

### Step 2

#### Output
```
=== Demo 3: Priority Inversion | USE_MUTEX 1 (mutex, priority inheritance) ===
[    0 ms] L: taking lock
[  100 ms] L: critical section 1/3, my priority = 1
[  101 ms] H: needs the lock
[  200 ms] L: critical section 2/3, my priority = 3
[  300 ms] L: critical section 3/3, my priority = 3
[  300 ms] H: got the lock after waiting 199 ms
[  350 ms] H: done
[  350 ms] M: starts 1000 ms of work (does not use the lock)
[ 1350 ms] M: finished
```
#### The waiting time reported by Task_H.
199 ms
#### The priority printed by Task_L.
3
#### The order in which Task_L, Task_M and Task_H complete their work.
Task_L -> Task_H -> Task_M

#### Table
| Observation | Binary Semaphore | Mutex |
|---|---|---|
| Task_H waiting time | 1189 ms | 199 ms |
| Task_L printed priority | 1 | 3 |
| Does Task_M delay Task_L? | Yes | No |
| Priority inheritance occur? | No | Yes |


### Step 3

#### Output
```
=== Demo 3: Priority Inversion | USE_MUTEX 0 (binary semaphore, NO inheritance) ===
[    0 ms] L: taking lock
[  100 ms] L: critical section 1/3, my priority = 1
[  101 ms] H: needs the lock
[  150 ms] M: starts 1000 ms of work (does not use the lock)
[ 2150 ms] M: finished
[ 2190 ms] L: critical section 2/3, my priority = 1
[ 2290 ms] L: critical section 3/3, my priority = 1
[ 2290 ms] H: got the lock after waiting 2189 ms
[ 2340 ms] H: done


=== Demo 3: Priority Inversion | USE_MUTEX 1 (mutex, priority inheritance) ===
[    0 ms] L: taking lock
[  100 ms] L: critical section 1/3, my priority = 1
[  101 ms] H: needs the lock
[  200 ms] L: critical section 2/3, my priority = 3
[  300 ms] L: critical section 3/3, my priority = 3
[  300 ms] H: got the lock after waiting 199 ms
[  350 ms] H: done
[  350 ms] M: starts 1000 ms of work (does not use the lock)
[ 2350 ms] M: finished
```

#### prediction
- Binary Semaphore: Task_H will wait about 1000 ms longer.
- Mutex: Task_H waiting time should stay the same because of priority inheritance.

#### Table 

| Observation | Binary Semaphore | Mutex |
|---|---|---|
| Task_M workload | 2000 ms | 2000 ms |
| Task_H waiting time (1000 ms workload) | 1189 ms | 199 ms |
| Task_H waiting time (2000 ms workload) | 2189 ms | 199 ms |
| Task_L priority | 1 | 3 |
| Priority inheritance | No | Yes |
| Does Task_M delay Task_H? | Yes | No |

## Part C

### Why does Task_H wait longer when a binary semaphore is used?
Because Task_M preempts Task_L. Because of that the Task__L can't release the lock.
### Explain how Task_L inherits the priority of Task_H when a mutex is used.
Task_L inherits the priority change because Task_L has the lock and Task_H is waiting with the higher priority.
### Why can Task_M no longer preempt Task_L while Task_L has inherited priority 3?
Because of the priority change Task_L has a higher priority than Task_M.
### What is the main difference between a binary semaphore and a mutex in this demonstration?
That of both ways only the mutex supports the priority inheritance. That reduces the waiting time of Task_H