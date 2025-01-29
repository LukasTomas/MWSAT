#!/usr/bin/env python3

import subprocess
import os
import sys
from pprint import pprint

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..', 'src')))
from run_experiment import run_experiment

DATA_DIR = os.path.join('..', '..', '..', 'data', '36-157', '.')

MUTATION_PROBABILITIES = [0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9]
SEEDS = 5
INSTANCES = 10

COMMAND_OPTIONS = {
    '--max-generations': '1000',
    '--population-size': '300',
    '--mutation-method': 'fixed',
    '--crossover': 'uniform',
    '--fitness-alpha': '0.9', 
    '--fitness-beta': '0.4',
    '--elitism': '0.4'
}

for option in ["M", "Q"]:
    print(option, "*************************************************************8")

    output_dir = os.path.join("results", option)
    os.makedirs(output_dir, exist_ok=True)

    for mutation_probab in MUTATION_PROBABILITIES:
        output_file = os.path.join(output_dir, f"fixed_mutation_{mutation_probab}.csv")
        
        COMMAND_OPTIONS['--mutation-probability'] = str(mutation_probab)
        pprint(COMMAND_OPTIONS)

        run_experiment(DATA_DIR, 123, SEEDS, INSTANCES, option, COMMAND_OPTIONS, output_file)
        print("==========================================\n\n")

    print("*************************************************************8")