#!/bin/bash

gnuplot --persist -e "set xlabel \"ms\"" \
                  -e "set title \"Schedule for S1, S2 and S3)\"" \
                  -e "set yrange [0:10]" \
                  -e "plot 'service0.dat' using 2:1 with lines title \"sequencer\", \
                           'service1.dat' using 2:1 with lines title \"frames\",
                           'service2.dat' using 2:1 with lines title \"select\",
                           'service3.dat' using 2:1 with lines title \"writeout\""
