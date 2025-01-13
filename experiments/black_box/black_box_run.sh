#!/bin/bash

COMMANDS=(
    "./run_experiment.py -d ../data/36-157/ -o M"
    "./run_experiment.py -d ../data/50-218/ -o M"
    "./run_experiment.py -d ../data/100-430/ -o M"
)

LOG_FILE=".black_box_run.log"
> "$LOG_FILE"

for cmd in "${COMMANDS[@]}"; do
    echo "====== Running: $cmd ======" | tee -a "$LOG_FILE"
    eval $cmd | tee -a "$LOG_FILE"
    echo "==========================================" | tee -a "$LOG_FILE"
    printf "\n\n\n" | tee -a "$LOG_FILE"
done