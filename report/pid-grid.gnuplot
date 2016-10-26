set term png
set output "pid-grid.png"

set title "Throughput vs Delay"
set xlabel "95-percentile signal delay (ms)"
set ylabel "Throughput (Mbit/s)"
set xrange [550:0]
set yrange [0:15.5]
unset key

plot "pid-grid.dat"
