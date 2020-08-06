#!/bin/sh

gnuplot --persist -e "set xlabel \"usec\"" \
                  -e "set ylabel \"Service#\"" \
                  -e "set title \"Schedule for S1, S2 and Sequencer (S4)\"" \
                  -e "set yrange [0:5]" \
                  -e "plot 'service1.dat' using 2:1 with lines, \
                           'service2.dat' using 2:1 with lines"
