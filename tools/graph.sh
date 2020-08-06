#!/bin/sh

# tips for the linear regressions (drift!)
# https://www.cs.grinnell.edu/~weinman/courses/CSC213/2008F/labs/10-pingpong-regression.pdf

#services
~/ecen5623/tools/graph_services.sh

#T jitter
~/ecen5623/tools/graph_S1_jitter.sh
~/ecen5623/tools/graph_S2_jitter.sh
~/ecen5623/tools/graph_S3_jitter.sh
~/ecen5623/tools/graph_frame_jitter.sh

#wcet jitter
~/ecen5623/tools/graph_C1_jitter.sh
~/ecen5623/tools/graph_C2_jitter.sh
~/ecen5623/tools/graph_C3_jitter.sh

