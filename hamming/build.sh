#!/bin/bash

rm -f hamming *.o
gcc -g hamming.c -o hamming -lm

rm -f hamming2 *.o
gcc -g hamming2.c -o hamming2 -lm
