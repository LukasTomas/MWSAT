#!/usr/bin/env python3

import subprocess
import os
import sys
from pprint import pprint

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..', 'src')))
from run_experiment import run_experiment

DATA_DIR = os.path.join('..', '..', '..', 'data', '20-71', '.')

ALPHA_VALUES = [0.15, 0.30, 0.45, 0.60, 0.75, 0.90]
SEEDS = 5
INSTANCES = 10

COMMAND_OPTIONS = {
    '--max-generations': '1000',
    '--population-size': '300',
    '--elitism': '0.5',
    '--mutation-probability': '0.3',
    '--crossover': 'uniform',
}

for option in ["M", "Q"]:
    output_dir = os.path.join("results", option)
    os.makedirs(output_dir, exist_ok=True)

    for alpha in ALPHA_VALUES:
        output_file = os.path.join(output_dir, f"alpha_{alpha}.csv")
        
        COMMAND_OPTIONS['--fitness-alpha'] = str(alpha)
        pprint(COMMAND_OPTIONS)

        run_experiment(DATA_DIR, 123, 5, 10, option, COMMAND_OPTIONS, output_file)
        print("==========================================\n\n")
