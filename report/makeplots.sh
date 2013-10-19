#!/bin/bash

#make tiger plot
gnuplot <<__EOF
set xlabel "cycle"
set ylabel "average reward"
set terminal epslatex
set output "plots/coinflip.eps"
set parametric
set trange [0:10000]
set xrange [0:10000]
plot "../log/coinflip1.csv" u 1:8  with lines title "coinflip first run", \
"../log/coinflip2.csv" u 1:8  with lines title "pretrained ct, no exploration", \
 t,0.7 title "optimal"
pause -1
__EOF
