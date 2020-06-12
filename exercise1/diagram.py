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
first_timestamp = None
last_timestamp = None
last_service = None

first_entry = True

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

            if first_entry is True:
               relative_offset = timestamp
               print("{0}".format(service), end = ' ')
               print(0, end = '\n')
               first_entry = False
               continue

# absolute
            current_timestamp = timestamp - relative_offset

            # This point completes each vertical line (transistion) on the timing plot
            if last_service != None:
                print("{0}".format(last_service), end = ' ')
                print(current_timestamp, end = '\n')
                
            print("{0}".format(service), end = ' ')
            print(current_timestamp, end = '\n')

            last_service = service
            last_timestamp = timestamp

        else:
            pass
