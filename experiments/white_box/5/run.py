#!/usr/bin/env python3

import subprocess
import os
import sys
from pprint import pprint
import pandas as pd

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..', 'src')))
from run_experiment import run_experiment

DATA_DIR = os.path.join('..', '..', '..', 'data')

POPULATION_SIZES = range(5, 200, 1)

OPTION =  'M'
SEEDS = 5
INSTANCES = 10

COMMAND_OPTIONS = {
    '--mutation-method': 'linear-adaptive',
    '--mutation-probability': '0.7',
    '--crossover': 'uniform',
    '--fitness-alpha': '0.9', 
    '--fitness-beta': '0.4',
    '--elitism': '0.4',
    '--max-generations': '300'
}

for dataset in ['20-71', '36-157', '50-218']:
    data_dir = os.path.join(DATA_DIR, dataset, '.')
    print(dataset, "*************************************************************8")

    output_dir = os.path.join("results", OPTION)
    os.makedirs(output_dir, exist_ok=True)

    results_for_df = []
    for population_size in POPULATION_SIZES:
        COMMAND_OPTIONS['--population-size'] = str(population_size)

        output_file = os.path.join(output_dir, f"popul{population_size}.csv")

        pprint(COMMAND_OPTIONS)

        run_experiment(data_dir, 123, SEEDS, INSTANCES, OPTION, COMMAND_OPTIONS, output_file)
        print("==========================================\n\n")

        df = pd.read_csv(output_file)
        relative_error = df['relative_error_avg'].mean()
        satisfied_clauses = df['satisfied_clauses'].mean()
        results_for_df.append({
            'population_size': population_size,
            'relative_error_avg': relative_error,
            'satisfied_clauses': satisfied_clauses
        })
    df = pd.DataFrame(results_for_df)
    df.to_csv(os.path.join("results", f"{dataset}.csv"))

    print("*************************************************************8")