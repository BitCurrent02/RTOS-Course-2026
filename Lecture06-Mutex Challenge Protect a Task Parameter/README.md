# Lecture 6 -Assignment - Mutex Challenge: Protect a Task Parameter
## Why can passing the address of a local variable to a task be unsafe?
After the function ends, its local variable is gone. The task could still have the old address. If it tries to read the value there, it could get the wrong value.
## When does blinkLED copy the parameter?
It copies the parameter at the beginning of the task. But aftter its taking the mutex and before releasing it again. 
## How does the mutex make setup() wait?
Because both tasks run on the Same Core and we gave blinkLED the higher priority, it runs before setup can continue with the lower priority.
## Why is this described as a demonstration hack? What would be a better design in a real application?
The trick in taht case is that blinkLED takes the mutex first. Therefor in a real program setup could send the value of the delay through the FreeRTOS queue.