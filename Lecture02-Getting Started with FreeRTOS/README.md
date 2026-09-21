ESP LAB 2:

Collaboration between Jihu Nam and Jan-Patrick Glaeser: 


Scenario A:
    Prediction before test:
	Task A is running because both have the same priority and task B is waiting for the end of Task A
    Actual observation:
	First task A, after this task B and this in a loop. When you change the order from xTaskCreatePinnedToCore always the Task which comes first in the code is the first one to execute.
    Did it match? Why?:
    	 yes it matched what we've expected.

Scenario B:
    Prediction before test:
	Task A is running first because it has the higher priority and task B is waiting for the end of Task A 
    Actual observation:
	First task A, after this task B and this in a loop. When you change the order from xTaskCreatePinnedToCore task A is always the first because of its priority.
    Did it match? Why?:
    	yes as we expected because Task A has the higher priority.

Scenario C:
    Prediction before test:
	Task B is running because it has the higher priority and task A is waiting for the end of Task B
    Actual observation:
	First task B, after this task A and this in a loop. When you change the order from xTaskCreatePinnedToCore task B is always the first because of its priority.
    Did it match? Why?:
    	yes as we expected because Task B has the higher priority.