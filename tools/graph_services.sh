#!/bin/sh

gnuplot --persist -e "set xlabel \"usec\"" \
                  -e "set title \"Schedule for S1, S2 and S3)\"" \
                  -e "set yrange [0:10]" \
                  -e "plot 'service1.dat' using 2:1 with lines, \
                           'service2.dat' using 2:1 with lines,
                           'service3.dat' using 2:1 with lines"

gnuplot --persist -e "set xlabel \"ms\"" \
                  -e "set ylabel \"jitter\"" \
                  -e "set yrange [38.5:41.5]" \
                  -e "set title \"Jitter for S1\"" \
                  -e "plot 'jitter_S1.dat' using 2:1 with lines"

gnuplot --persist -e "set xlabel \"ms\"" \
                  -e "set ylabel \"jitter\"" \
                  -e "set title \"Jitter for S2\"" \
                  -e "plot 'jitter_S2.dat' using 2:1 with lines"

gnuplot --persist -e "set xlabel \"ms\"" \
                  -e "set ylabel \"jitter\"" \
                  -e "set title \"Jitter S3\"" \
                  -e "plot 'jitter_S3.dat' using 2:1 with lines"

gnuplot --persist -e "set xlabel \"ms\"" \
                  -e "set ylabel \"jitter\"" \
                  -e "set title \"Frame Jitter\"" \
                  -e "plot 'jitter_frames.dat' using 2:1 with lines"
