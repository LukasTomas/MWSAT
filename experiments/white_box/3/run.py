#!/usr/bin/env python3

import subprocess
import os
import sys
from pprint import pprint

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..', 'src')))
from run_experiment import run_experiment

DATA_DIR = os.path.join('..', '..', '..', 'data', '20-71', '.')

ELISTISM_VALUES = range(0, 1, 0.1)
SEEDS = 5
INSTANCES = 10

COMMAND_OPTIONS = {
    '--max-generations': '1000',
    '--population-size': '300',
    '--mutation-probability': '0.3',
    '--crossover': 'uniform',
    '--fitness-alpha': '0.9', 
    '--fitness-beta': '0.4',
}

for option in ["M", "Q"]:
    print(option, "*************************************************************8")

    output_dir = os.path.join("results", option)
    os.makedirs(output_dir, exist_ok=True)

    for elitism in ELISTISM_VALUES:
        output_file = os.path.join(output_dir, f"elitism_{elitism}.csv")
        
        COMMAND_OPTIONS['--elitism'] = str(elitism)
        pprint(COMMAND_OPTIONS)

        run_experiment(DATA_DIR, 123, 5, 10, option, COMMAND_OPTIONS, output_file)
        print("==========================================\n\n")

    print("*************************************************************8")