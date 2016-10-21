# 6.829 Problem Set 2
**Anish Athalye / Kate Yu**

# Exercise A

Here is the data we collected

| Window | Throughput (Mbit/s) | 95-percentile delay (ms) | Score |
| ------ | ------------------- | ------------------------ | ----- |
| 5      | 1.08                | 108                      | 10.00 |
| 10     | 1.99                | 155                      | 12.83 |
| 20     | 3.32                | 278                      | 11.94 |
| 30     | 4.11                | 402                      | 10.22 |
| 50     | 4.80                | 609                      | 7.88  |
| 90     | 5.01                | 975                      | 5.14  |
| 100    | 5.02                | 1054                     | 4.76  |

## Reproducability

We found the results to be very reproducible. We ran the simulation in a
virtual machine on GCP (while not running anything else on the machine). When
re-running the simulation with a window size of 50, for example, we got a
throughput of exactly 4.80 Mbit/s and a 95-percentile delay of 609ms between
runs.

# Exercise B

# Exercise C

# Exercise D

# Exercise E
