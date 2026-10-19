# Lecture 4 - Lab 1: Memory Detective

## Table 
| Measruements | Free Heap |
|---|---|
| Before Task A 4096 | 359112 b |
| After Task A 4096 | 354384 b |
| After Task A 4096 | 349656 b |
|---|---|
| Before Task A 8192 | 359112 b |
| After Task A 8192 | 354384 b |
| After Task B 8192 | 349656 b |

### Serial Monitor 4096 B
![4096 B Stack](4096%20B%20Stack.png)

### Serial Monitor 8192 B
![8192 B Stack](8192%20B%20Stack.png)


## Comparison and Questions

### a. Did the free heap change when you increased the task stack size?
yes. The free heap decreseased. It decreased when I incresead the size of the stack from 4096 to 8192.

### b. What happened to the free heap?
It decreasee because both tasks needed more memory. The available heap decreased from 349656 to 341208 after I created both tasks.

### c. Why does a task need stack memory?
It needs stack memory to store the local variables and function calls information.

### d. Why does creating a FreeRTOS task use RAM?
It uses RAM because each of these tasks needs its own stack and an own Task Control Block. That happens when you allocate this memory from the heap.

## Serial Monitor 4096 B
![4096 B Stack](4096%20B%20Stack.png)

## Serial Monitor 8192 B
![8192 B Stack](8192%20B%20Stack.png)
