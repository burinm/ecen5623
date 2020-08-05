#!/bin/bash

cat ~/frames/*.log | sort -n > master.log

cat master.log | ./diagram_services.py MEMLOG_E_S1_RUN MEMLOG_E_S1_DONE 1 > service1.dat 
cat master.log | ./diagram_services.py MEMLOG_E_S2_RUN MEMLOG_E_S2_DONE 2 > service2.dat 
cat master.log | ./diagram_services.py MEMLOG_E_S3_RUN MEMLOG_E_S3_DONE 3 > service3.dat 
