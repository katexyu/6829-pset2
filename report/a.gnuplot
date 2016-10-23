set term png
set output "a.png"

set title "Throughput vs Delay"
set xlabel "95-percentile signal delay (ms)"
set ylabel "Throughput (Mbit/s)"
set xrange [1100:0]
set yrange [0:5.5]
unset key

plot "a.dat"
