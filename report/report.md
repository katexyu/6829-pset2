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

We implemented a delay triggered scheme with a target delay centered around 100ms,
with a 20ms tolerance in either direction. In terms of window size adjustment, we
experimented with different schemes and parameters, as shown in the following table:

A corresponds to the additive factor, and B corresponds to the multiplicative factor.

| Scheme | A | B | Throughput (Mbit/s) | 95-percentile delay (ms) | Score |
| ------ | - | - | ------------------- | ------------------------ | ----- |
| AIMD   | 1 | 2 | 3.04                | 134                      | 22.69 |
| AIAD   | 1 |   | 4.77                | 466                      | 10.24 |
| MIAD   | 1 | 2 | 4.94                | 18041                    | 0.27  |
| MIMD   |   | 2 | 4.71                | 7873                     | 0.59  |

Our AIMD-esque implementation performed well, resulting in a score of 22.69.
We decided to further experiment with the AIMD scheme by tuning the parameters.

| Lower (ms) | Upper (ms) | A | B | Throughput (Mbit/s) | 95-percentile delay (ms) | Score |
| ---------- | ---------- | - | - | ------------------- | ------------------------ | ----- |
| 80         | 120        | 1 | 2 | 3.04                | 134                      | 22.69 |
| 60         | 100        | 1 | 2 | 2.66                | 128                      | 20.78 |
| 100        | 140        | 1 | 2 | 3.46                | 158                      | 21.90 |
| 90         | 110        | 1 | 2 | 2.92                | 134                      | 21.79 |
| 70         | 110        | 1 | 2 | 2.91                | 134                      | 21.72 |
| 70         | 130        | 1 | 2 | 3.17                | 145                      | 21.86 |
| 80         | 120        | 1 | 3 | 3.00                | 138                      | 21.74 |
| 80         | 120        | 2 | 2 | 3.53                | 157                      | 22.48 |

The best result was still achieved by the AIMD implementation centered around 100ms with a
20ms tolerance, a additive factor of 1, and a multiplicative factor of 2.

# Exercise D

## PID

We implemented a PID controller to control window size.

### References

* http://www.controleng.com/single-article/fixing-pid/3975cad3f121d8df3fc0fd67660822b1.html

# Exercise E
