#!/bin/bash

DURATION=$1
if [ $# -ne 1 ]; then
  DURATION=6
fi

plymouthd --kernel-command-line="quiet splash"
plymouth show-splash 
plymouth display-message --text="Press enter to reboot"
for ((I=0; I<$DURATION; I++)); do
  plymouth --update=test$I;
  sleep 1;
  done;
plymouth quit
