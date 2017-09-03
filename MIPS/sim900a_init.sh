#!/bin/sh
cd /sys/class/gpio
echo 11 > export
cd gpio11
echo out > direction
echo "GPIO Set OK"
