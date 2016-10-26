set term png
set output "pid-vs-pd-grid.png"

set title "Throughput vs Delay"
set xlabel "95-percentile signal delay (ms)"
set ylabel "Throughput (Mbit/s)"
set xrange [3500:0]
# set yrange [0:15.38]

set key outside
plot "pd-grid.dat" title "PD", "pid-grid.dat" title "PID"
