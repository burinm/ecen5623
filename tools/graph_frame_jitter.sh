#!/bin/bash

gnuplot --persist -e "set xlabel \"ms\"" \
                  -e "set ylabel \"jitter ms\"" \
                  -e "set title \"Frame Jitter\"" \
                  -e "f(x) = m*x + b" \
                  -e "fit f(x) 'jitter_frames.dat' using 2:1 via m,b" \
                  -e "plot 'jitter_frames.dat' using 2:1 with lines linetype 4, \
                  f(x) title sprintf(\"y = %.5fx + %.2f\", m, b)"

#For printing slope!
# https://stackoverflow.com/questions/9070166/when-using-gnuplot-how-can-the-equation-of-a-line-be-printed-in-the-line-title
