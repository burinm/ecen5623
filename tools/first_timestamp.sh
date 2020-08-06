#!/bin/bash
head -1 master.log | awk '{print $1}'
