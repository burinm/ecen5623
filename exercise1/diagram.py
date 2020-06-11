#!/usr/bin/env python3

import sys

if len(sys.argv) != 2:
    print("usage: diagram <timing_file.txt>"); 
    sys.exit(0)


data_file = None

try:
    data_file = open(sys.argv[1], "r")
except:
    print("Couldn't open", sys.argv[1]); 
    sys.exit(0)
    
timestamp = None
last_timestamp = None
last_service = None

for l in data_file:
    l = l.strip();
    if l[:1] == '#':
        continue 
    else:
        values = l.split()
        if len(values) == 2:
            (timestamp, service) = values

            # Timestamps are in ns, convert to usec
            timestamp = int((int(timestamp) / 1000))

            if last_timestamp != None:
                print(timestamp - last_timestamp, end = ' ')

            if last_service != service:
                print("S{0}".format(service), end = ' ')
            

            last_service = service
            last_timestamp = timestamp

        else:
            pass
