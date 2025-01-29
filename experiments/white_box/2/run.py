#!/usr/bin/env python3

import subprocess
import os
import sys
from pprint import pprint

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..', 'src')))
from run_experiment import run_experiment

DATA_DIR = os.path.join('..', '..', '..', 'data', '20-71', '.')

ALPHA = [0.6, 0.7, 0.8, 0.9]
BETA = [0.1, 0.2, 0.3, 0.4]
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
    print(option, "*************************************************************8")

    output_dir = os.path.join("results", option)
    os.makedirs(output_dir, exist_ok=True)

    for alpha in ALPHA:
        for beta in BETA:
            output_file = os.path.join(output_dir, f"alpha_{alpha}_beta_{beta}.csv")
            
            COMMAND_OPTIONS['--fitness-alpha'] = str(alpha)
            COMMAND_OPTIONS['--fitness-beta'] = str(beta)
            pprint(COMMAND_OPTIONS)

            run_experiment(DATA_DIR, 123, 5, 10, option, COMMAND_OPTIONS, output_file)
            print("==========================================\n\n")

    print("*************************************************************8")