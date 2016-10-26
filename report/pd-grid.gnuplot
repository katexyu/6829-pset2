set term png
set output "pd-grid.png"

set title "Throughput vs Delay"
set xlabel "95-percentile signal delay (ms)"
set ylabel "Throughput (Mbit/s)"
set xrange [3500:0]
set yrange [0:15.38]
unset key

plot "pd-grid.dat"
