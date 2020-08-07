#!/bin/bash

egrep -a -e  "^#" ./frames/* > cross_frames.txt
echo "cross_frames.txt written"
