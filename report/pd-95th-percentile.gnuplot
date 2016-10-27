set term png
set output "pd-95th-percentile.png"

set title "Throughput vs Delay"
set xlabel "95-percentile signal delay (ms)"
set ylabel "Throughput (Mbit/s)"
set xrange [250:0]
set yrange [0:12]
unset key

plot "pd-95th-percentile.dat"
