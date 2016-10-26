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

## PID Controller

We implemented a PID controller to control window size to keep EWMAed RTT close
to a given target. This isn't optimizing for exactly the right thing (we wanted
to have a low 95th percentile signal delay), but it's still reasonably good.

We also added some code to prevent "reset windup", which is a condition that
results in a large accumulated negative error. This is an issue with the system
because window size can't be "driven" to a negative value.

We didn't really know a particularly principled way of setting PID parameters,
so we manually tuned parameters. To avoid "overfitting" the PID parameters, we
trained on the TMobile dataset and tested on the Verizon dataset. Here is the
data from testing **on the TMobile dataset**:

| Target (ms) | EWMA | K_P  | K_I  | K_D  | Throughput | Delay | Score |
| ----------- | ---- | ---- | ---- | ---- | -----------| ----- | ----- |
| 90          | 0.2  | 1e-1 | 1e-2 | 2e-3 | 14.27      | 268   | 53.25 |
| 90          | 0.4  | 1e-1 | 1e-2 | 2e-3 | 14.29      | 268   | 53.32 |
| 90          | 0.2  | 1e-1 | 2e-2 | 2e-3 | 12.66      | 302   | 41.92 |

### Grid Search

After this, we decided to do a grid search over a larger parameter space (see
`gridsearch.py`):

```python
SPACE = {
    'RTT_EWMA_FACTOR': ['0.2', '0.4'],
    'TARGET_DELAY': ['90.0'],
    'K_P': ['1e0', '5e-1', '1e-1', '5e-2', '1e-2'],
    'K_I': ['1e-1', '5e-2', '1e-2', '5e-3', '1e-3'],
    'K_D': ['5e-2', '1e-2', '5e-3', '1e-3', '5e-4'],
}
```

From 250 experiments, here are the bottom 5 and top 5 results:

| EWMA | Target | K_D  | K_P  | K_I  | Throughput | Delay | Score |
| ---- | ------ | ---- | ---- | ---- | ---------- | ----- | ----- |
| 0.4  | 90.0   | 1e-2 | 1e-2 | 1e-1 | 11.17      | 521   | 21.43 |
| 0.2  | 90.0   | 1e-2 | 1e-2 | 1e-1 | 11.31      | 526   | 21.50 |
| 0.4  | 90.0   | 1e-2 | 5e-2 | 1e-1 | 11.46      | 520   | 22.03 |
| 0.2  | 90.0   | 1e-3 | 1e-2 | 1e-1 | 11.11      | 502   | 22.13 |
| 0.2  | 90.0   | 1e-3 | 5e-2 | 1e-1 | 11.27      | 509   | 22.14 |
| 0.2  | 90.0   | 5e-4 | 1e-2 | 1e-2 | 13.14      | 232   | 56.63 |
| 0.2  | 90.0   | 5e-2 | 1e-2 | 1e-2 | 13.39      | 236   | 56.73 |
| 0.4  | 90.0   | 5e-2 | 1e-2 | 1e-2 | 13.40      | 236   | 56.77 |
| 0.2  | 90.0   | 5e-3 | 1e-2 | 1e-2 | 13.16      | 231   | 56.96 |
| 0.2  | 90.0   | 1e-2 | 1e-2 | 1e-2 | 13.24      | 231   | 57.31 |

Taking the best parameters and running the simulation on the Verizon trace, we
get a throughput of 4.33 Mbits/s and a 95th percentile signal delay of 186
milliseconds for a score of 23.28.

If we do the grid search on the Verizon trace directly, we find the parameters
EWMA = 0.4, Target = 90, K_I = 5e-3, K_D = 1e-2, and K_P = 1e-2 for a
throughput of 4.58 Mbits/s, 95th percentile signal delay of 179 ms, and a score
of 25.59.

Next, we tried doing a search to find the best target delay, keeping EWMA =
0.2, K_P = 1e-2, K_I = 1e-2, and K_D = 1e-2. Here are the results:

| Target | Throughput | Delay | Score |
| ------ | ---------- | ----- | ----- |
| 110    | 4.46       | 226   | 19.73 |
| 100    | 4.42       | 206   | 21.46 |
| 90     | 4.33       | 186   | 23.28 |
| 80     | 4.19       | 186   | 25.39 |
| 60     | 3.22       | 122   | 26.39 |
| 70     | 3.94       | 138   | 28.55 |

We tried running this on the TMobile trace (as a sanity check to see if we were
overfitting), and we got a throughput of 10.55 Mbits/s, 95th percentile signal
delay of 198 ms, and a score of 53.28.

### References

* http://www.controleng.com/single-article/fixing-pid/3975cad3f121d8df3fc0fd67660822b1.html

## PIDish Controller

We found that when there were large changes in network capacity, the PID
controller did not react quickly enough. In particular, when network capacity
decreased rapidly, our controller didn't back off quickly enough. To account
for this, we tried modifying PID so that we had two different K_D values, one
for negative derivatives and one for positive derivatives.

This didn't actually make much of a difference.

## PD Controller

We tried using a PD controller to control changes to the window size.

## PD Controller with Multiplicative Decrease

We tried augmenting our PD controller with our multiplicative decrease implementation from
Part B. With no tuning, using a timeout of 1000ms and a MD factor of 2.0, the performance was
pretty bad (1.09 Mbits/s throughput and 112 ms 95th percentile signal delay on the Verizon
trace).

We then ran a gridsearch over MD factors and non-1 MD_FACTORS performed strictly worse than when no
multiplicative decrease was applied. From our results, it seemed like the multiplicative decrease
factor caused our controller to become far too conservative. These are some of the best results from
the gridsearch:

| K_P  | K_D  | Target Delay | M_D Factor | Throughput | Delay | Score |
|------|----- | ------------ | ---------- | ---------- | ----- | ----- |
| 1e-2 | 1e-2 | 80.0         | 1.1        | 1.72       | 52    | 33.08 |
| 1e-2 | 1e-2 | 70.0         | 1.0        | 10.92      | 197   | 55.43 |
| 1e-2 | 1e-2 | 80.0         | 1.0        | 12.35      | 218   | 56.65 |
| 1e-2 | 1e-2 | 80.0         | 1.0        | 12.37      | 218   | 56.74 |

# Exercise E

# Contributions

We pair programmed exercises A, B, and C. We came up with the idea for the PID
controller together. Anish wrote the grid search.
