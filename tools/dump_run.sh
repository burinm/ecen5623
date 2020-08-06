#!/bin/bash


DIR=`date +%m-%d-%Y-%H-%M`

mkdir ${DIR} || exit -1
cd ${DIR} || exit -1



MAKE_TIMESTAMPS=~/synchronome/simple-capture/make_timestamps.sh

echo "get logs"
scp 10.0.0.17:~/synchronome/simple-capture/frame.log .
scp 10.0.0.17:~/synchronome/simple-capture/processing.log .
scp 10.0.0.17:~/synchronome/simple-capture/writeout.log .

echo "copying frames"
scp -r 10.0.0.17:~/synchronome/simple-capture/frames/ .


echo "Creating timestamps.log"
${MAKE_TIMESTAMPS} ./frames ppm > timestamps.log

cd -
