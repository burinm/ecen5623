#!/bin/bash

DIAGRAM_FILTER=~/ecen5623/tools/diagram_filter.py
TIME_DIFF=~/synchronome/simple-capture/wcet/timediff.py

if [ "$#" -ne 2 ]; then
    echo "usage: make_services2_graph_data <start_ms> <end_ms>"
    exit 0
fi

START_TIME=$1
FINISH_TIME=$2


DIAGRAM_SERVICES=~/ecen5623/tools/diagram_services.py


echo "get data from pi"
#scp 10.0.0.17:~/experimental/synchronome/simple-capture/*.log .
#scp -r 10.0.0.17:~/experimental/synchronome/simple-capture/frames .

cat  sequencer.log frame.log processing.log writeout.log | sort -n > master.log

FIRST_TIMESTAMP=`~/ecen5623/tools/first_timestamp.sh`

echo "create timestamp log"
#~/synchronome/simple-capture/make_timestamps.sh ./frames ppm > timestamps.log

echo "Creating service2 run data"
cat master.log | ${DIAGRAM_SERVICES} SEQUENCER_MARK SEQUENCER_MARK_DONE 1 $1 $2 ${FIRST_TIMESTAMP} > service0.dat
cat master.log | ${DIAGRAM_SERVICES} MEMLOG_E_S1_RUN MEMLOG_E_S1_DONE 2 $1 $2 ${FIRST_TIMESTAMP} > service1.dat
cat master.log | ${DIAGRAM_SERVICES} MEMLOG_E_S2_RUN MEMLOG_E_S2_DONE 3 $1 $2 ${FIRST_TIMESTAMP} > service2.dat
cat master.log | ${DIAGRAM_SERVICES} MEMLOG_E_S3_RUN MEMLOG_E_S3_DONE 4 $1 $2 ${FIRST_TIMESTAMP} > service3.dat

# jitters
set -x
echo "Creating S0 jitter data"
${TIME_DIFF} master.log SEQUENCER_MARK | awk '{print $1 " " $3}' | ${DIAGRAM_FILTER} $1 $2 ${FIRST_TIMESTAMP} > jitter_S0.dat

echo "Creating S1 jitter data"
${TIME_DIFF} master.log MEMLOG_E_S1_DONE | awk '{print $1 " " $3}' | ${DIAGRAM_FILTER} $1 $2 ${FIRST_TIMESTAMP} > jitter_S1.dat

echo "Creating S2 jitter data"
${TIME_DIFF} master.log MEMLOG_E_S2_DONE | awk '{print $1 " " $3}' | ${DIAGRAM_FILTER} $1 $2 ${FIRST_TIMESTAMP} > jitter_S2.dat

echo "Creating S3 jitter data"
${TIME_DIFF} master.log MEMLOG_E_S3_DONE | awk '{print $1 " " $3}' | ${DIAGRAM_FILTER} $1 $2 ${FIRST_TIMESTAMP} > jitter_S3.dat

#wcets
echo "Creating C0 jitter data"
${TIME_DIFF} frame.log  | grep SEQUENCER_MARK_DONE | awk '{print $1 " " $3}' | ${DIAGRAM_FILTER} $1 $2 ${FIRST_TIMESTAMP} > jitter_C0.dat

echo "Creating C1 jitter data"
${TIME_DIFF} frame.log MEMLOG_E_WCET_START MEMLOG_E_S1_DONE | grep MEMLOG_E_S1_DONE | awk '{print $1 " " $3}' | ${DIAGRAM_FILTER} $1 $2 ${FIRST_TIMESTAMP} > jitter_C1.dat

echo "Creating C2 jitter data"
${TIME_DIFF} processing.log MEMLOG_E_WCET_START MEMLOG_E_S2_DONE | grep MEMLOG_E_S2_DONE | awk '{print $1 " " $3}' | ${DIAGRAM_FILTER} $1 $2 ${FIRST_TIMESTAMP} > jitter_C2.dat

echo "Creating C3 jitter data"
${TIME_DIFF} writeout.log MEMLOG_E_WCET_START MEMLOG_E_S3_DONE | grep MEMLOG_E_S3_DONE | awk '{print $1 " " $3}' | ${DIAGRAM_FILTER} $1 $2 ${FIRST_TIMESTAMP} > jitter_C3.dat

echo "Creating Frame jitter data"
${TIME_DIFF} timestamps.log | awk '{print $1 " " $3}' | ${DIAGRAM_FILTER} $1 $2 ${FIRST_TIMESTAMP} > jitter_frames.dat


