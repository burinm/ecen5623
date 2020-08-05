#!/bin/bash

if [ "$#" -ne 2 ]; then
    echo "usage: make_graphs <start_ms> <end_ms>"
    exit 0
fi

START_TIME=$1
FINISH_TIME=$2

scp 10.0.0.17:~/synchronome/simple-capture/*.log .
cat *.log | sort -n > master.log

cat master.log | ./diagram_services.py MEMLOG_E_S1_RUN MEMLOG_E_S1_DONE 1 $1 $2 > service1.dat 
cat master.log | ./diagram_services.py MEMLOG_E_S2_RUN MEMLOG_E_S2_DONE 2 $1 $2 > service2.dat 
cat master.log | ./diagram_services.py MEMLOG_E_S3_RUN MEMLOG_E_S3_DONE 3 $1 $2 > service3.dat 

# jitters
~/synchronome/simple-capture/wcet/timediff.py master.log MEMLOG_E_S1_DONE | awk '{print $1 " " $3}' | ./diagram_filter.py $1 $2 > jitter_S1.dat

~/synchronome/simple-capture/wcet/timediff.py master.log MEMLOG_E_S2_DONE | awk '{print $1 " " $3}' | ./diagram_filter.py $1 $2 > jitter_S2.dat

~/synchronome/simple-capture/wcet/timediff.py master.log MEMLOG_E_S3_DONE | awk '{print $1 " " $3}' | ./diagram_filter.py $1 $2 > jitter_S3.dat

#ssh 10.0.0.17 -C ~/synchronome/simple-capture/make_timestamps.sh ~/synchronome/simple-capture/frames ppm > timestamps.log
#scp 10.0.0.17:~/synchronome/simple-capture/timestamps.log .
~/synchronome/simple-capture/make_timestamps.sh ~/frames ppm > timestamps.log
~/synchronome/simple-capture/wcet/timediff.py timestamps.log | awk '{print $1 " " $3}' | ./diagram_filter.py $1 $2 > jitter_frames.dat
