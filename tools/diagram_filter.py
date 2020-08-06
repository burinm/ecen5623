#!/usr/bin/env python3

# digram.py - burin (c) 2020
#   Output datapoints for a GNUplot graph from timing data

import sys

def time_in_ms(timestamp_string):
    (sec, nsec) = timestamp_string.split(".")
    # Put Timestamps ms
    timestamp = int(sec) * 1000
    timestamp += int(nsec) / 1000000

    return timestamp

if not (len(sys.argv) == 3 or len(sys.argv) == 4):
    print("usage: diagram_filter start_ms finish_ms <absolute>");
    sys.exit(0)

start_ms = int(sys.argv[1])
finish_ms = int(sys.argv[2])

absolute_timestamp = None
if len(sys.argv) == 4:
    absolute_timestamp = sys.argv[3]


data_file = None

try:
    data_file = sys.stdin
except:
    print("Couldn't open <stdin>"); 
    sys.exit(0)
    
timestamp = None
first_timestamp = None

first_entry = True

for l in data_file:
    l = l.strip();
    if l[:1] == '#':
        continue 
    else:
        values = l.split()
        if len(values) >= 2:
            timestamp_string = values[0]
            jitter = values[1]


            timestamp = time_in_ms(timestamp_string)


            if first_entry is True:
                if absolute_timestamp is None:
                   relative_offset = timestamp
                   first_entry = False
                else:
                    relative_offset = time_in_ms(absolute_timestamp)
                    first_entry = False

            # absolute
            current_timestamp = timestamp - relative_offset

            if current_timestamp > start_ms and current_timestamp < finish_ms:
                print("{0}".format(jitter), end = ' ')
                print("{:.6f}".format(current_timestamp), end = '\n')
