# Lecture 9 - Assignment - Timer Events in a Queue

## Run A
- Timer interval: 100ms
- Task delay: 20 ms
- Queue length: 4

### Serial output:
```
ESP-ROM:esp32s3-20210327
Build:Mar 27 2021
rst:0x1 (POWERON),boot:0x8 (SPI_FAST_FLASH_BOOT)
SPIWP:0xee
mode:DIO, clock div:1
load:0x3fce3808,len:0x370
load:0x403c9700,len:0x900
load:0x403cc700,len:0x2364
entry 0x403c98ac
1
2
3
4
5
6
7
8
9
10
11
12
```
## Run B
- Timer interval: 100ms
- Task delay: 500 ms
- Queue length: 4

### Prediction
My prediction is that because we set the task delay to 500 ms, it works for the first 4 values but when the queue is full, it starts skipping numbers. That means there will be gaps in the output. During the 500 ms task delay, I expect five new numbers to arrive (500 ms / 100 ms = 5). The queue can hold only four numbers at a time, so I expect gaps in the output.

### Output 
```
1
2
3
4
5
6
11
16
21
26
31
36
```
### Results
My prediction was partly correct. The gaps which I expected appeared, but later than I expected. The numbers 1-6 appeared in order and between 6 and 11 is the gap. That means when 7,8,9 and 10 arrived was the queue full and because of that these numbers were lost. The numbers 3 to 6 were already in the queue, so the task printed them in FIFO order before it printed 11.