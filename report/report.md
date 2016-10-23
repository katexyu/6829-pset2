# 6.829 Problem Set 2
**Anish Athalye / Kate Yu**

# Exercise A

Here is the data we collected

| Window | Throughput (Mbit/s) | 95-percentile delay (ms) | Score |
| ------ | ------------------- | ------------------------ | ----- |
| 5      | 1.08                | 108                      | 10.00 |
| 10     | 1.99                | 155                      | 12.83 |
| 15     | 2.72                | 214                      | 12.71 |
| 20     | 3.32                | 278                      | 11.94 |
| 30     | 4.11                | 402                      | 10.22 |
| 40     | 4.55                | 505                      | 9.01  |
| 50     | 4.80                | 609                      | 7.88  |
| 60     | 4.92                | 714                      | 6.89  |
| 70     | 4.97                | 811                      | 6.13  |
| 80     | 4.99                | 897                      | 5.56  |
| 90     | 5.01                | 975                      | 5.14  |
| 100    | 5.02                | 1054                     | 4.76  |

![](a.png)\


## Best window size

We found that the best window size (among our data points) is 10 (for a score
of 12.83).

## Reproducability

We found the results to be very reproducible. We ran the simulation in a
virtual machine on GCP (while not running anything else on the machine). When
re-running the simulation with a window size of 50, for example, we got a
throughput of exactly 4.80 Mbit/s and a 95-percentile delay of 609ms between
runs.

# Exercise B

We implemented a simple TCP-like AIMD scheme. Initially, we chose 1 as our
additive increase parameter (A) and 2 as our multiplicative decrease parameter
(B). That is, we increased our window size by 1/(window size) upon receiving
each ack and we decreased our window size by a factor of 2 every time we
detected a dropped packet.

We did some experiments with varying A, B, and a hard-coded timeout parameter.

| Timeout | A | B | Throughput (Mbit/s) | 95-percentile delay (ms) | Score |
| ------- | - | - | ------------------- | ------------------------ | ----- |
| 1000    | 1 | 2 | 4.83                | 892                      | 5.41  |
| 1000    | 3 | 2 | 4.95                | 1167                     | 4.21  |
| 1000    | 1 | 4 | 4.84                | 893                      | 5.42  |
| 500     | 1 | 4 | 4.73                | 480                      | 9.85  |

Next, we implemented EWMA RTT estimation with weight EWMA (a parameter) and
updated the implementation to consider a packet to be lost if no ack was
received for LATE (another parameter) times the estimated RTT.

| LATE | EWMA | A | B | Throughput (Mbit/s) | 95-percentile delay (ms) | Score |
| ---- | ---- | - | - | ------------------- | ------------------------ | ----- |
| 1.5  | 0.1  | 1 | 2 | 3.42                | 612                      | 5.59  |
| 1.5  | 0.2  | 1 | 2 | 4.69                | 1306                     | 3.59  |
| 1.5  | 0.5  | 1 | 2 | 4.91                | 1648                     | 2.98  |
| 1.1  | 0.2  | 1 | 2 | 0.92                | 115                      | 8.00  |
| 1.2  | 0.2  | 1 | 2 | 1.45                | 123                      | 11.79 |

In our experiments, AIMD didn't give an improvement over static window sizes.
Compared to choosing a static window size (where we did a search over the
window size space, perhaps overfitting the data), AIMD did worse at maximizing
the target metric. However, AIMD can do pretty well at maximizing throughput
(as we would expect).

# Exercise C

# Exercise D

# Exercise E
