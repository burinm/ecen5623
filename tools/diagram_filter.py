#!/usr/bin/env python3

# digram.py - burin (c) 2020
#   Output datapoints for a GNUplot graph from timing data

import sys

if len(sys.argv) != 3:
    print("usage: diagram_filter start_ms finish_ms"); 
    sys.exit(0)

start_ms = int(sys.argv[1])
finish_ms = int(sys.argv[2])

data_file = None

try:
    data_file = sys.stdin
except:
    print("Couldn't open <stdin>"); 
    sys.exit(0)
    
timestamp = None
first_timestamp = None
last_timestamp = None

first_entry = True

for l in data_file:
    l = l.strip();
    if l[:1] == '#':
        continue 
    else:
        values = l.split()
        if len(values) == 2:
            timestamp = values[0]
            jitter = values[1]

            (sec, nsec) = timestamp.split(".")

            # Put Timestamps ms 
            timestamp = int(sec) * 1000
            timestamp += int(nsec) / 1000000


            if first_entry is True:
               relative_offset = timestamp
               first_entry = False
               # plot 0,0 as first datum
               continue

            # absolute
            current_timestamp = timestamp - relative_offset

            found = False 

            # if last_timestamp is not None:
            if current_timestamp > start_ms and current_timestamp < finish_ms:
                print("{0}".format(jitter), end = ' ')
                print(current_timestamp, end = '\n')
