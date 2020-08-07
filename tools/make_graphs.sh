#!/bin/bash

DIAGRAM_FILTER=~/ecen5623/tools/diagram_filter.py
DIAGRAM_SERVICES=~/ecen5623/tools/diagram_services.py
TIME_DIFF=~/synchronome/simple-capture/wcet/timediff.py

if [ "$#" -ne 2 ]; then
    echo "usage: make_graphs <start_ms> <end_ms>"
    exit 0
fi

START_TIME=$1
FINISH_TIME=$2

# Chicken / Egg problem with first_timestamp.sh
cat  frame.log processing.log writeout.log | sort -n > master.log

FIRST_TIMESTAMP=`~/ecen5623/tools/first_timestamp.sh`

rm -f fit.log
rm -f *.dat

#scp 10.0.0.17:~/synchronome/simple-capture/frame.log .
#scp 10.0.0.17:~/synchronome/simple-capture/processing.log .
#scp 10.0.0.17:~/synchronome/simple-capture/writeout.log .


echo "Creating service run data"
cat master.log | ${DIAGRAM_SERVICES} MEMLOG_E_S1_RUN MEMLOG_E_S1_DONE 1 $1 $2 ${FIRST_TIMESTAMP} > service1.dat
cat master.log | ${DIAGRAM_SERVICES} MEMLOG_E_S2_RUN MEMLOG_E_S2_DONE 2 $1 $2 ${FIRST_TIMESTAMP} > service2.dat
cat master.log | ${DIAGRAM_SERVICES} MEMLOG_E_S3_RUN MEMLOG_E_S3_DONE 3 $1 $2 ${FIRST_TIMESTAMP} > service3.dat

# jitters
echo "Creating S1 jitter data"
${TIME_DIFF} master.log MEMLOG_E_S1_DONE | awk '{print $1 " " $3}' | ${DIAGRAM_FILTER} $1 $2 ${FIRST_TIMESTAMP} > jitter_S1.dat

echo "Creating S2 jitter data"
${TIME_DIFF} master.log MEMLOG_E_S2_DONE | awk '{print $1 " " $3}' | ${DIAGRAM_FILTER} $1 $2 ${FIRST_TIMESTAMP} > jitter_S2.dat

echo "Creating S3 jitter data"
${TIME_DIFF} master.log MEMLOG_E_S3_DONE | awk '{print $1 " " $3}' | ${DIAGRAM_FILTER} $1 $2 ${FIRST_TIMESTAMP} > jitter_S3.dat

#echo "Creating timestamps.log"
#ssh 10.0.0.17 -C ~/synchronome/simple-capture/make_timestamps.sh ~/synchronome/simple-capture/frames ppm > timestamps.log
#scp 10.0.0.17:~/synchronome/simple-capture/timestamps.log .
#~/synchronome/simple-capture/make_timestamps.sh ./frames ppm > timestamps.log
echo "Creating Frame jitter data"
${TIME_DIFF} timestamps.log | awk '{print $1 " " $3}' | ${DIAGRAM_FILTER} $1 $2 ${FIRST_TIMESTAMP} > jitter_frames.dat

#wcets
echo "Creating C1 jitter data"
${TIME_DIFF} frame.log MEMLOG_E_WCET_START MEMLOG_E_S1_DONE | grep MEMLOG_E_S1_DONE | awk '{print $1 " " $3}' | ${DIAGRAM_FILTER} $1 $2 ${FIRST_TIMESTAMP} > jitter_C1.dat

echo "Creating C2 jitter data"
${TIME_DIFF} processing.log MEMLOG_E_WCET_START MEMLOG_E_S2_DONE | grep MEMLOG_E_S2_DONE | awk '{print $1 " " $3}' | ${DIAGRAM_FILTER} $1 $2 ${FIRST_TIMESTAMP} > jitter_C2.dat

echo "Creating C3 jitter data"
${TIME_DIFF} writeout.log MEMLOG_E_WCET_START MEMLOG_E_S3_DONE | grep MEMLOG_E_S3_DONE | awk '{print $1 " " $3}' | ${DIAGRAM_FILTER} $1 $2 ${FIRST_TIMESTAMP} > jitter_C3.dat
