#!/bin/bash

rm -f hamming *.o
gcc -g hamming.c -o hamming -lm
