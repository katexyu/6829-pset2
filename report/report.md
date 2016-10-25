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

We implemented a PID controller to control window size. We also added some code
to prevent "reset windup", which is a condition that results in a large
accumulated negative error. This is an issue with the system because window
size can't be "driven" to a negative value.

We didn't really know a particularly principled way of setting PID parameters,
so we manually tuned parameters. To avoid "overfitting" the PID parameters, we
trained on the TMobile dataset and tested on the Verizon dataset. Here is the
data from testing **on the TMobile dataset**:

| Target (ms) | EWMA | K_P  | K_I  | K_D  | Throughput (Mbit/s) | 95-percentile delay (ms) | Score |
| ----------- | ---- | ---- | ---- | ---- | ------------------- | ------------------------ | ----- |
| 90          | 0.2  | 1e-1 | 1e-2 | 2e-3 | 14.27               | 268                      | 53.25 |
| 90          | 0.4  | 1e-1 | 1e-2 | 2e-3 | 14.29               | 268                      | 53.32 |
| 90          | 0.2  | 1e-1 | 2e-2 | 2e-3 | 12.66               | 302                      | 41.92 |

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

Here are the results:

```
params, throughput, delay, score
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '1e-2', 'K_P': '1e-2', 'K_I': '1e-1'}, 11.17, 521, 21.43953934740883)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '1e-2', 'K_P': '1e-2', 'K_I': '1e-1'}, 11.31, 526, 21.50190114068441)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '1e-2', 'K_P': '5e-2', 'K_I': '1e-1'}, 11.46, 520, 22.03846153846154)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '1e-3', 'K_P': '1e-2', 'K_I': '1e-1'}, 11.11, 502, 22.131474103585656)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '1e-3', 'K_P': '5e-2', 'K_I': '1e-1'}, 11.27, 509, 22.141453831041257)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-3', 'K_P': '5e-1', 'K_I': '1e-1'}, 12.07, 541, 22.31053604436229)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '1e-3', 'K_P': '1e-2', 'K_I': '1e-1'}, 10.95, 489, 22.392638036809817)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-3', 'K_P': '1e-1', 'K_I': '1e-1'}, 11.67, 520, 22.442307692307693)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-4', 'K_P': '1e-2', 'K_I': '1e-1'}, 10.98, 483, 22.732919254658384)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '1e-3', 'K_P': '1e-1', 'K_I': '1e-1'}, 11.56, 508, 22.755905511811022)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-4', 'K_P': '1e0', 'K_I': '1e-1'}, 12.38, 543, 22.79926335174954)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-4', 'K_P': '1e0', 'K_I': '1e-1'}, 12.32, 537, 22.942271880819366)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-2', 'K_P': '1e-2', 'K_I': '1e-1'}, 11.48, 499, 23.006012024048097)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-4', 'K_P': '5e-2', 'K_I': '1e-1'}, 11.37, 493, 23.06288032454361)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-4', 'K_P': '1e-2', 'K_I': '1e-1'}, 10.93, 468, 23.354700854700855)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-2', 'K_P': '1e-1', 'K_I': '1e-1'}, 11.73, 502, 23.366533864541832)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-3', 'K_P': '1e-2', 'K_I': '1e-1'}, 11.13, 476, 23.38235294117647)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-2', 'K_P': '5e-2', 'K_I': '1e-1'}, 11.78, 498, 23.65461847389558)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-4', 'K_P': '5e-1', 'K_I': '1e-1'}, 12.2, 513, 23.78167641325536)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '1e-2', 'K_P': '1e-1', 'K_I': '1e-1'}, 11.68, 490, 23.836734693877553)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-3', 'K_P': '1e-1', 'K_I': '1e-1'}, 11.76, 491, 23.95112016293279)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-4', 'K_P': '5e-1', 'K_I': '1e-1'}, 12.11, 503, 24.07554671968191)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-2', 'K_P': '5e-2', 'K_I': '1e-1'}, 11.61, 482, 24.08713692946058)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-4', 'K_P': '1e-1', 'K_I': '1e-1'}, 11.61, 481, 24.137214137214137)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '1e-2', 'K_P': '1e-1', 'K_I': '1e-1'}, 11.63, 478, 24.330543933054393)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '1e-3', 'K_P': '5e-2', 'K_I': '1e-1'}, 11.4, 468, 24.358974358974358)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-4', 'K_P': '1e-1', 'K_I': '1e-1'}, 11.54, 472, 24.449152542372882)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '1e-2', 'K_P': '5e-1', 'K_I': '1e-1'}, 12.24, 500, 24.48)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-3', 'K_P': '1e0', 'K_I': '1e-1'}, 12.2, 497, 24.547283702213278)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '1e-2', 'K_P': '5e-2', 'K_I': '1e-1'}, 11.28, 456, 24.736842105263158)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '1e-3', 'K_P': '5e-1', 'K_I': '1e-1'}, 12.15, 488, 24.897540983606557)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-2', 'K_P': '1e-2', 'K_I': '1e-1'}, 11.36, 455, 24.967032967032967)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-3', 'K_P': '5e-2', 'K_I': '1e-1'}, 11.44, 458, 24.978165938864628)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '1e-3', 'K_P': '1e-1', 'K_I': '1e-1'}, 11.6, 463, 25.053995680345572)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '1e-2', 'K_P': '1e0', 'K_I': '1e-1'}, 12.42, 492, 25.24390243902439)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-3', 'K_P': '1e-2', 'K_I': '1e-1'}, 10.99, 433, 25.381062355658198)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-4', 'K_P': '5e-2', 'K_I': '1e-1'}, 11.16, 437, 25.537757437070937)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '1e-3', 'K_P': '1e0', 'K_I': '1e-1'}, 12.24, 476, 25.714285714285715)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-2', 'K_P': '1e0', 'K_I': '1e-1'}, 12.36, 480, 25.75)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-2', 'K_P': '5e-1', 'K_I': '1e-1'}, 12.09, 469, 25.778251599147122)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-2', 'K_P': '5e-1', 'K_I': '1e-1'}, 12.12, 470, 25.78723404255319)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-2', 'K_P': '1e-1', 'K_I': '1e-1'}, 11.76, 455, 25.846153846153847)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '1e-2', 'K_P': '1e0', 'K_I': '1e-1'}, 12.41, 480, 25.854166666666668)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '1e-3', 'K_P': '5e-1', 'K_I': '1e-1'}, 12.07, 462, 26.125541125541126)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-3', 'K_P': '5e-2', 'K_I': '1e-1'}, 11.34, 428, 26.49532710280374)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-3', 'K_P': '1e0', 'K_I': '1e-1'}, 12.29, 462, 26.6017316017316)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '1e-2', 'K_P': '5e-1', 'K_I': '1e-1'}, 12.17, 455, 26.747252747252748)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-3', 'K_P': '1e-2', 'K_I': '5e-2'}, 9.85, 363, 27.134986225895318)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '1e-3', 'K_P': '1e0', 'K_I': '1e-1'}, 12.26, 451, 27.184035476718403)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-4', 'K_P': '1e-1', 'K_I': '5e-2'}, 10.8, 395, 27.341772151898734)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-2', 'K_P': '1e0', 'K_I': '1e-1'}, 12.28, 449, 27.34966592427617)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-3', 'K_P': '5e-2', 'K_I': '5e-2'}, 10.28, 375, 27.413333333333334)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-4', 'K_P': '1e-2', 'K_I': '5e-2'}, 9.87, 359, 27.493036211699163)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-4', 'K_P': '5e-2', 'K_I': '5e-2'}, 10.34, 374, 27.647058823529413)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-2', 'K_P': '1e-2', 'K_I': '5e-2'}, 10.32, 371, 27.816711590296496)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-2', 'K_P': '1e0', 'K_I': '5e-2'}, 12.13, 432, 28.078703703703702)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-3', 'K_P': '5e-1', 'K_I': '1e-1'}, 12.14, 430, 28.232558139534884)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '1e-2', 'K_P': '5e-2', 'K_I': '5e-2'}, 10.3, 363, 28.37465564738292)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '1e-3', 'K_P': '1e-1', 'K_I': '5e-2'}, 10.7, 373, 28.68632707774799)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '1e-3', 'K_P': '1e-2', 'K_I': '5e-2'}, 9.83, 342, 28.742690058479532)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '1e-2', 'K_P': '1e-2', 'K_I': '5e-2'}, 10.12, 351, 28.83190883190883)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '1e-3', 'K_P': '5e-2', 'K_I': '5e-2'}, 10.35, 358, 28.910614525139664)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-4', 'K_P': '1e-2', 'K_I': '5e-2'}, 9.78, 338, 28.93491124260355)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-4', 'K_P': '1e0', 'K_I': '5e-2'}, 12.11, 418, 28.971291866028707)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-3', 'K_P': '1e-2', 'K_I': '5e-2'}, 10.0, 344, 29.069767441860463)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-4', 'K_P': '5e-2', 'K_I': '5e-2'}, 10.33, 354, 29.180790960451976)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '1e-2', 'K_P': '1e-2', 'K_I': '5e-2'}, 9.99, 341, 29.296187683284458)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-2', 'K_P': '5e-1', 'K_I': '5e-2'}, 11.89, 401, 29.65087281795511)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '1e-3', 'K_P': '5e-2', 'K_I': '5e-2'}, 10.27, 344, 29.8546511627907)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '1e-2', 'K_P': '1e-1', 'K_I': '5e-2'}, 10.69, 357, 29.943977591036415)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-3', 'K_P': '1e-1', 'K_I': '5e-2'}, 10.75, 359, 29.944289693593316)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '1e-3', 'K_P': '1e-2', 'K_I': '5e-2'}, 9.91, 326, 30.39877300613497)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-2', 'K_P': '1e0', 'K_I': '5e-2'}, 12.14, 399, 30.426065162907268)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-4', 'K_P': '1e-1', 'K_I': '5e-2'}, 10.62, 349, 30.429799426934096)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '1e-2', 'K_P': '5e-1', 'K_I': '5e-2'}, 11.67, 381, 30.62992125984252)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '1e-3', 'K_P': '1e0', 'K_I': '5e-2'}, 12.18, 395, 30.835443037974684)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-3', 'K_P': '5e-1', 'K_I': '5e-2'}, 11.81, 383, 30.8355091383812)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-2', 'K_P': '5e-2', 'K_I': '5e-2'}, 10.62, 344, 30.872093023255815)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '1e-2', 'K_P': '5e-2', 'K_I': '5e-2'}, 10.38, 336, 30.892857142857142)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-4', 'K_P': '5e-1', 'K_I': '5e-2'}, 11.81, 382, 30.916230366492147)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-3', 'K_P': '5e-2', 'K_I': '5e-2'}, 10.4, 335, 31.044776119402986)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-2', 'K_P': '5e-2', 'K_I': '5e-2'}, 10.53, 339, 31.061946902654867)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '1e-3', 'K_P': '5e-1', 'K_I': '5e-2'}, 11.77, 378, 31.137566137566136)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-2', 'K_P': '5e-1', 'K_I': '5e-2'}, 11.85, 380, 31.18421052631579)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '1e-3', 'K_P': '1e-1', 'K_I': '5e-2'}, 10.75, 344, 31.25)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-4', 'K_P': '1e0', 'K_I': '5e-2'}, 12.27, 392, 31.301020408163264)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-2', 'K_P': '1e-2', 'K_I': '5e-2'}, 10.33, 330, 31.303030303030305)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-2', 'K_P': '1e-1', 'K_I': '5e-2'}, 10.99, 351, 31.31054131054131)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '1e-2', 'K_P': '1e-1', 'K_I': '5e-2'}, 10.75, 341, 31.524926686217007)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '1e-2', 'K_P': '1e0', 'K_I': '5e-2'}, 12.14, 385, 31.532467532467532)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-3', 'K_P': '5e-1', 'K_I': '5e-2'}, 11.81, 373, 31.66219839142091)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '1e-2', 'K_P': '1e0', 'K_I': '5e-2'}, 12.19, 385, 31.662337662337663)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-3', 'K_P': '1e-1', 'K_I': '5e-2'}, 10.81, 341, 31.700879765395893)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-3', 'K_P': '1e0', 'K_I': '5e-2'}, 12.24, 384, 31.875)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-2', 'K_P': '1e-1', 'K_I': '5e-2'}, 10.97, 343, 31.982507288629737)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '1e-3', 'K_P': '5e-1', 'K_I': '5e-2'}, 11.79, 368, 32.03804347826087)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '1e-3', 'K_P': '1e0', 'K_I': '5e-2'}, 12.28, 383, 32.06266318537859)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-3', 'K_P': '1e0', 'K_I': '5e-2'}, 12.25, 378, 32.407407407407405)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '1e-2', 'K_P': '5e-1', 'K_I': '5e-2'}, 11.69, 346, 33.786127167630056)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-4', 'K_P': '5e-1', 'K_I': '5e-2'}, 11.89, 343, 34.66472303206997)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '1e-2', 'K_P': '1e0', 'K_I': '1e-3'}, 14.93, 383, 38.98172323759791)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '1e-3', 'K_P': '1e0', 'K_I': '1e-3'}, 14.97, 383, 39.08616187989556)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-2', 'K_P': '1e0', 'K_I': '1e-3'}, 14.87, 380, 39.13157894736842)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-3', 'K_P': '1e0', 'K_I': '1e-3'}, 14.91, 380, 39.23684210526316)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-3', 'K_P': '1e0', 'K_I': '1e-3'}, 14.92, 376, 39.680851063829785)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '1e-3', 'K_P': '1e0', 'K_I': '1e-3'}, 14.97, 376, 39.81382978723404)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-4', 'K_P': '1e0', 'K_I': '1e-3'}, 14.95, 369, 40.51490514905149)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-2', 'K_P': '1e0', 'K_I': '1e-3'}, 14.86, 361, 41.16343490304709)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-4', 'K_P': '1e0', 'K_I': '1e-3'}, 14.94, 362, 41.270718232044196)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-2', 'K_P': '1e0', 'K_I': '1e-2'}, 14.77, 350, 42.2)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-2', 'K_P': '1e0', 'K_I': '1e-2'}, 14.67, 346, 42.39884393063584)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '1e-2', 'K_P': '1e0', 'K_I': '1e-3'}, 14.92, 351, 42.50712250712251)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-2', 'K_P': '1e0', 'K_I': '5e-3'}, 14.84, 348, 42.64367816091954)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '1e-3', 'K_P': '1e0', 'K_I': '1e-2'}, 14.73, 345, 42.69565217391305)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-3', 'K_P': '1e0', 'K_I': '5e-3'}, 14.84, 346, 42.89017341040462)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '1e-2', 'K_P': '1e0', 'K_I': '1e-2'}, 14.77, 344, 42.93604651162791)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '1e-2', 'K_P': '1e0', 'K_I': '1e-2'}, 14.76, 343, 43.032069970845484)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-4', 'K_P': '1e0', 'K_I': '5e-3'}, 14.88, 345, 43.130434782608695)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-4', 'K_P': '1e0', 'K_I': '1e-2'}, 14.75, 340, 43.38235294117647)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '1e-3', 'K_P': '5e-1', 'K_I': '1e-3'}, 15.29, 351, 43.56125356125356)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-2', 'K_P': '5e-1', 'K_I': '1e-3'}, 15.26, 350, 43.6)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '1e-2', 'K_P': '5e-1', 'K_I': '1e-3'}, 15.27, 349, 43.75358166189112)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '1e-3', 'K_P': '5e-1', 'K_I': '1e-3'}, 15.3, 349, 43.83954154727794)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-4', 'K_P': '5e-1', 'K_I': '1e-3'}, 15.28, 348, 43.9080459770115)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-4', 'K_P': '5e-1', 'K_I': '1e-3'}, 15.31, 347, 44.12103746397695)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '1e-2', 'K_P': '1e0', 'K_I': '5e-3'}, 14.91, 337, 44.2433234421365)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-3', 'K_P': '1e0', 'K_I': '5e-3'}, 14.93, 337, 44.3026706231454)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-4', 'K_P': '1e0', 'K_I': '1e-2'}, 14.71, 332, 44.30722891566265)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '1e-2', 'K_P': '1e0', 'K_I': '5e-3'}, 14.91, 336, 44.375)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '1e-3', 'K_P': '1e0', 'K_I': '5e-3'}, 14.91, 336, 44.375)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-3', 'K_P': '1e0', 'K_I': '1e-2'}, 14.75, 332, 44.4277108433735)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-3', 'K_P': '1e0', 'K_I': '1e-2'}, 14.72, 330, 44.60606060606061)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-2', 'K_P': '1e0', 'K_I': '5e-3'}, 14.77, 330, 44.75757575757576)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-3', 'K_P': '5e-1', 'K_I': '1e-3'}, 15.28, 341, 44.80938416422288)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '1e-3', 'K_P': '1e0', 'K_I': '5e-3'}, 14.9, 332, 44.87951807228916)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-4', 'K_P': '1e0', 'K_I': '5e-3'}, 14.93, 332, 44.96987951807229)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-2', 'K_P': '5e-1', 'K_I': '1e-3'}, 15.25, 338, 45.11834319526627)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-3', 'K_P': '5e-1', 'K_I': '1e-3'}, 15.31, 338, 45.29585798816568)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-2', 'K_P': '5e-1', 'K_I': '1e-2'}, 14.8, 326, 45.39877300613497)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-4', 'K_P': '1e-2', 'K_I': '1e-3'}, 12.77, 281, 45.444839857651246)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '1e-3', 'K_P': '1e0', 'K_I': '1e-2'}, 14.69, 323, 45.47987616099071)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '1e-2', 'K_P': '5e-1', 'K_I': '1e-3'}, 15.33, 337, 45.489614243323444)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-3', 'K_P': '1e-2', 'K_I': '1e-3'}, 12.79, 281, 45.51601423487544)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-4', 'K_P': '1e-2', 'K_I': '1e-3'}, 12.79, 281, 45.51601423487544)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-3', 'K_P': '1e-2', 'K_I': '1e-3'}, 12.8, 281, 45.55160142348755)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '1e-3', 'K_P': '5e-1', 'K_I': '1e-2'}, 14.86, 326, 45.58282208588957)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '1e-2', 'K_P': '1e-2', 'K_I': '1e-3'}, 12.82, 281, 45.62277580071174)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-2', 'K_P': '1e-2', 'K_I': '1e-3'}, 12.87, 282, 45.638297872340424)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '1e-3', 'K_P': '1e-2', 'K_I': '1e-3'}, 12.83, 281, 45.65836298932384)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-2', 'K_P': '1e-2', 'K_I': '1e-3'}, 12.84, 281, 45.693950177935946)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '1e-2', 'K_P': '1e-2', 'K_I': '1e-3'}, 12.84, 281, 45.693950177935946)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '1e-3', 'K_P': '1e-2', 'K_I': '1e-3'}, 12.84, 281, 45.693950177935946)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-3', 'K_P': '5e-1', 'K_I': '1e-2'}, 14.86, 325, 45.723076923076924)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '1e-2', 'K_P': '5e-1', 'K_I': '1e-2'}, 14.87, 323, 46.037151702786375)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '1e-2', 'K_P': '5e-1', 'K_I': '5e-3'}, 15.29, 332, 46.05421686746988)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-4', 'K_P': '5e-2', 'K_I': '1e-3'}, 14.35, 310, 46.29032258064516)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '1e-3', 'K_P': '5e-2', 'K_I': '1e-3'}, 14.29, 308, 46.396103896103895)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '1e-2', 'K_P': '5e-1', 'K_I': '1e-2'}, 14.86, 320, 46.4375)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-4', 'K_P': '5e-1', 'K_I': '1e-2'}, 14.87, 320, 46.46875)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-3', 'K_P': '5e-2', 'K_I': '1e-3'}, 14.36, 309, 46.47249190938511)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '1e-2', 'K_P': '5e-2', 'K_I': '1e-3'}, 14.38, 309, 46.53721682847896)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '1e-2', 'K_P': '5e-2', 'K_I': '1e-3'}, 14.34, 308, 46.55844155844156)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-3', 'K_P': '5e-1', 'K_I': '1e-2'}, 14.87, 319, 46.61442006269593)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '1e-3', 'K_P': '5e-2', 'K_I': '1e-3'}, 14.36, 308, 46.62337662337662)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '1e-3', 'K_P': '5e-1', 'K_I': '5e-3'}, 15.28, 327, 46.72782874617737)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-4', 'K_P': '5e-1', 'K_I': '1e-2'}, 14.86, 318, 46.729559748427675)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-4', 'K_P': '1e-1', 'K_I': '1e-3'}, 14.87, 318, 46.76100628930818)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-4', 'K_P': '5e-2', 'K_I': '1e-3'}, 14.37, 307, 46.807817589576544)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '1e-3', 'K_P': '1e-1', 'K_I': '1e-3'}, 14.84, 317, 46.813880126182966)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-3', 'K_P': '5e-2', 'K_I': '1e-3'}, 14.42, 308, 46.81818181818182)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-3', 'K_P': '1e-1', 'K_I': '1e-3'}, 14.86, 317, 46.87697160883281)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-3', 'K_P': '1e-1', 'K_I': '1e-3'}, 14.86, 317, 46.87697160883281)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-2', 'K_P': '1e-1', 'K_I': '1e-3'}, 14.87, 317, 46.90851735015773)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-4', 'K_P': '1e-1', 'K_I': '1e-3'}, 14.88, 317, 46.94006309148265)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '1e-2', 'K_P': '1e-1', 'K_I': '1e-3'}, 14.85, 316, 46.99367088607595)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-2', 'K_P': '1e-1', 'K_I': '1e-3'}, 14.85, 316, 46.99367088607595)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '1e-2', 'K_P': '1e-1', 'K_I': '1e-3'}, 14.86, 316, 47.0253164556962)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '1e-3', 'K_P': '5e-1', 'K_I': '1e-2'}, 14.86, 316, 47.0253164556962)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '1e-3', 'K_P': '1e-1', 'K_I': '1e-3'}, 14.88, 316, 47.08860759493671)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-2', 'K_P': '5e-2', 'K_I': '1e-3'}, 14.29, 303, 47.161716171617165)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-4', 'K_P': '5e-1', 'K_I': '5e-3'}, 15.25, 323, 47.21362229102167)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-2', 'K_P': '5e-1', 'K_I': '1e-2'}, 14.79, 313, 47.25239616613418)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-2', 'K_P': '5e-1', 'K_I': '5e-3'}, 15.2, 321, 47.35202492211838)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-2', 'K_P': '5e-1', 'K_I': '5e-3'}, 15.21, 321, 47.38317757009346)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-2', 'K_P': '5e-2', 'K_I': '1e-3'}, 14.36, 302, 47.549668874172184)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-4', 'K_P': '5e-1', 'K_I': '5e-3'}, 15.29, 318, 48.081761006289305)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-3', 'K_P': '5e-1', 'K_I': '5e-3'}, 15.24, 315, 48.38095238095238)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '1e-3', 'K_P': '5e-1', 'K_I': '5e-3'}, 15.27, 311, 49.09967845659164)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '1e-2', 'K_P': '5e-1', 'K_I': '5e-3'}, 15.26, 308, 49.54545454545455)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-3', 'K_P': '5e-1', 'K_I': '5e-3'}, 15.25, 301, 50.66445182724252)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '1e-2', 'K_P': '1e-1', 'K_I': '1e-2'}, 14.3, 271, 52.76752767527675)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-3', 'K_P': '1e-1', 'K_I': '1e-2'}, 14.3, 271, 52.76752767527675)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-4', 'K_P': '1e-1', 'K_I': '1e-2'}, 14.3, 271, 52.76752767527675)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '1e-2', 'K_P': '1e-1', 'K_I': '1e-2'}, 14.31, 271, 52.80442804428044)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '1e-2', 'K_P': '1e-1', 'K_I': '5e-3'}, 15.05, 285, 52.80701754385965)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-2', 'K_P': '1e-1', 'K_I': '5e-3'}, 15.05, 284, 52.99295774647887)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-3', 'K_P': '1e-1', 'K_I': '1e-2'}, 14.27, 269, 53.048327137546465)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-2', 'K_P': '1e-1', 'K_I': '5e-3'}, 15.06, 283, 53.21554770318021)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '1e-3', 'K_P': '1e-1', 'K_I': '1e-2'}, 14.27, 268, 53.24626865671642)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '1e-3', 'K_P': '1e-1', 'K_I': '1e-2'}, 14.27, 268, 53.24626865671642)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-4', 'K_P': '1e-1', 'K_I': '1e-2'}, 14.3, 266, 53.7593984962406)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-2', 'K_P': '5e-2', 'K_I': '5e-3'}, 14.83, 275, 53.92727272727273)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '1e-3', 'K_P': '1e-1', 'K_I': '5e-3'}, 15.05, 278, 54.13669064748201)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '1e-2', 'K_P': '5e-2', 'K_I': '5e-3'}, 14.86, 274, 54.23357664233577)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '1e-2', 'K_P': '5e-2', 'K_I': '5e-3'}, 14.87, 274, 54.27007299270073)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-3', 'K_P': '1e-2', 'K_I': '5e-3'}, 14.17, 261, 54.2911877394636)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-2', 'K_P': '5e-2', 'K_I': '5e-3'}, 14.83, 273, 54.32234432234432)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-3', 'K_P': '5e-2', 'K_I': '5e-3'}, 14.83, 273, 54.32234432234432)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-4', 'K_P': '1e-1', 'K_I': '5e-3'}, 15.05, 277, 54.332129963898915)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '1e-3', 'K_P': '1e-1', 'K_I': '5e-3'}, 15.05, 277, 54.332129963898915)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '1e-3', 'K_P': '5e-2', 'K_I': '5e-3'}, 14.84, 273, 54.35897435897436)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '1e-3', 'K_P': '5e-2', 'K_I': '5e-3'}, 14.84, 273, 54.35897435897436)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-2', 'K_P': '1e-1', 'K_I': '1e-2'}, 14.31, 263, 54.4106463878327)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-4', 'K_P': '5e-2', 'K_I': '5e-3'}, 14.87, 273, 54.46886446886447)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-3', 'K_P': '5e-2', 'K_I': '5e-3'}, 14.85, 272, 54.595588235294116)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-4', 'K_P': '1e-1', 'K_I': '5e-3'}, 15.07, 276, 54.60144927536232)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '1e-3', 'K_P': '1e-2', 'K_I': '5e-3'}, 14.17, 259, 54.71042471042471)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-4', 'K_P': '5e-2', 'K_I': '5e-3'}, 14.83, 271, 54.723247232472325)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '1e-2', 'K_P': '1e-1', 'K_I': '5e-3'}, 15.05, 275, 54.72727272727273)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '1e-2', 'K_P': '1e-2', 'K_I': '5e-3'}, 14.21, 259, 54.86486486486486)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-3', 'K_P': '5e-2', 'K_I': '1e-2'}, 13.94, 254, 54.881889763779526)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-3', 'K_P': '1e-1', 'K_I': '5e-3'}, 15.04, 274, 54.89051094890511)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-4', 'K_P': '1e-2', 'K_I': '5e-3'}, 14.18, 258, 54.96124031007752)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '1e-2', 'K_P': '1e-2', 'K_I': '5e-3'}, 14.19, 258, 55.0)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-3', 'K_P': '1e-1', 'K_I': '5e-3'}, 15.02, 273, 55.01831501831502)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '1e-2', 'K_P': '5e-2', 'K_I': '1e-2'}, 13.99, 254, 55.07874015748032)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-4', 'K_P': '5e-2', 'K_I': '1e-2'}, 13.94, 253, 55.098814229249015)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-4', 'K_P': '1e-2', 'K_I': '5e-3'}, 14.17, 257, 55.13618677042802)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-2', 'K_P': '5e-2', 'K_I': '1e-2'}, 14.06, 255, 55.13725490196079)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '1e-2', 'K_P': '5e-2', 'K_I': '1e-2'}, 13.95, 253, 55.13833992094862)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-3', 'K_P': '1e-2', 'K_I': '5e-3'}, 14.18, 256, 55.390625)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '1e-3', 'K_P': '5e-2', 'K_I': '1e-2'}, 13.92, 251, 55.45816733067729)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-4', 'K_P': '1e-2', 'K_I': '1e-2'}, 13.15, 237, 55.48523206751055)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '1e-3', 'K_P': '5e-2', 'K_I': '1e-2'}, 13.93, 251, 55.49800796812749)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-3', 'K_P': '5e-2', 'K_I': '1e-2'}, 14.0, 252, 55.55555555555556)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-4', 'K_P': '5e-2', 'K_I': '1e-2'}, 13.96, 251, 55.61752988047809)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-2', 'K_P': '5e-2', 'K_I': '1e-2'}, 14.03, 252, 55.67460317460318)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-2', 'K_P': '1e-1', 'K_I': '1e-2'}, 14.26, 256, 55.703125)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-2', 'K_P': '1e-2', 'K_I': '5e-3'}, 14.29, 256, 55.8203125)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-2', 'K_P': '1e-2', 'K_I': '5e-3'}, 14.26, 255, 55.92156862745098)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '1e-3', 'K_P': '1e-2', 'K_I': '5e-3'}, 14.18, 253, 56.047430830039524)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '1e-2', 'K_P': '1e-2', 'K_I': '1e-2'}, 13.2, 235, 56.170212765957444)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '1e-3', 'K_P': '1e-2', 'K_I': '1e-2'}, 13.09, 233, 56.18025751072961)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-3', 'K_P': '1e-2', 'K_I': '1e-2'}, 13.11, 233, 56.26609442060086)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '1e-3', 'K_P': '1e-2', 'K_I': '1e-2'}, 13.07, 232, 56.33620689655172)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-4', 'K_P': '1e-2', 'K_I': '1e-2'}, 13.14, 232, 56.63793103448276)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-2', 'K_P': '1e-2', 'K_I': '1e-2'}, 13.39, 236, 56.73728813559322)
({'RTT_EWMA_FACTOR': '0.4', 'TARGET_DELAY': '90.0', 'K_D': '5e-2', 'K_P': '1e-2', 'K_I': '1e-2'}, 13.4, 236, 56.779661016949156)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '5e-3', 'K_P': '1e-2', 'K_I': '1e-2'}, 13.16, 231, 56.96969696969697)
({'RTT_EWMA_FACTOR': '0.2', 'TARGET_DELAY': '90.0', 'K_D': '1e-2', 'K_P': '1e-2', 'K_I': '1e-2'}, 13.24, 231, 57.316017316017316)
```

### References

* http://www.controleng.com/single-article/fixing-pid/3975cad3f121d8df3fc0fd67660822b1.html

# Exercise E
