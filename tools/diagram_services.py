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

if not (len(sys.argv) == 6 or len(sys.argv) == 7):
    print("usage: diagram SERVICE_START SERVICE_FINISH NUM start_ms finish_ms <absolute>");
    sys.exit(0)


service_start = sys.argv[1]
service_finish = sys.argv[2]
service_num = int(sys.argv[3])

start_ms = int(sys.argv[4])
finish_ms = int(sys.argv[5])

absolute_timestamp = None
if len(sys.argv) == 7:
    absolute_timestamp = sys.argv[6]

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
        if len(values) >= 2:
            timestamp_string = values[0]
            service = values[1]

            timestamp= time_in_ms(timestamp_string)

            if first_entry is True:
                if absolute_timestamp is None:
                   relative_offset = timestamp
                   first_entry = False
                else:
                    relative_offset = time_in_ms(absolute_timestamp)
                    first_entry = False


            # absolute
            current_timestamp = timestamp - relative_offset

            found = False

            # if last_timestamp is not None:
            if current_timestamp > start_ms and current_timestamp < finish_ms:
                # This point completes each vertical line (transistion) on the timing plot
                if service == service_start:
                    service_num_start = service_num * 2
                    service_num_finish = service_num * 2 - 1
                    found = True

                if service == service_finish:
                    service_num_start = service_num * 2 - 1
                    service_num_finish = service_num * 2
                    found = True

                if found == True:
                    print("{0}".format(service_num_finish), end = ' ')
                    print(current_timestamp, end = '\n')

                    print("{0}".format(service_num_start), end = ' ')
                    print(current_timestamp, end = '\n')
