#!/bin/bash

CALL_DIR=$PWD
SCRIPT_RELATIVE_PATH=$(dirname "$0")

cd $SCRIPT_RELATIVE_PATH

scp *.py pi@192.168.1.4:/home/pi/Apps/


