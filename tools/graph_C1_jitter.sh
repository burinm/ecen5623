#!/bin/bash

gnuplot --persist -e "set xlabel \"ms\"" \
                  -e "set ylabel \"jitter ms\"" \
                  -e "set yrange [.5:2]" \
                  -e "set title \"Jitter for C1 (frame capture) WCET\"" \
                  -e "f(x) = m*x + b" \
                  -e "fit f(x) 'jitter_C1.dat' using 2:1 via m,b" \
                  -e "plot 'jitter_C1.dat' using 2:1 with lines, \
                  f(x) title sprintf(\"y = %.5fx + %.2f\", m, b)"

#For printing slope!
# https://stackoverflow.com/questions/9070166/when-using-gnuplot-how-can-the-equation-of-a-line-be-printed-in-the-line-title
