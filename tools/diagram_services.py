#!/usr/bin/env python3

# digram.py - burin (c) 2020
#   Output datapoints for a GNUplot graph from timing data

import sys

if len(sys.argv) != 4:
    print("usage: diagram SERVICE_START SERVICE_FINISH NUM"); 
    sys.exit(0)

service_start = sys.argv[1]
service_finish = sys.argv[2]
service_num = sys.argv[3]

data_file = None

try:
    data_file = sys.stdin
except:
    print("Couldn't open <stdin>"); 
    sys.exit(0)
    
timestamp = None
first_timestamp = None
last_timestamp = None
last_service = None

service_num_start = None
service_num_finish = None

first_entry = True

for l in data_file:
    l = l.strip();
    if l[:1] == '#':
        continue 
    else:
        values = l.split()
        timestamp = values[0]
        service = values[1]

        # print(timestamp, service)

        (sec, nsec) = timestamp.split(".")

        # Put Timestamps ms 
        timestamp = int(sec) * 1000
        timestamp += int(nsec) / 1000000


        if first_entry is True:
           relative_offset = timestamp
           # print("{0}".format(service), end = ' ')
           # print(0, end = '\n')
           first_entry = False
           # plot 0,0 as first datum
           continue

        # absolute
        current_timestamp = timestamp - relative_offset

        found = False 

        # if last_timestamp is not None:
        if current_timestamp < 20000:  # for now only graph up to 1 seconds
            # This point completes each vertical line (transistion) on the timing plot
            if service == service_start: 
                service_num_start = service_num 
                service_num_finish = 0
                found = True 

            if service == service_finish: 
                service_num_start = 0
                service_num_finish = service_num
                found = True 

            if found == True:    
                print("{0}".format(service_num_finish), end = ' ')
                print(current_timestamp, end = '\n')

                print("{0}".format(service_num_start), end = ' ')
                print(current_timestamp, end = '\n')

        # last_timestamp = current_timestamp
