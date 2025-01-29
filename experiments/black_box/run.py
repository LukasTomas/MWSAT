#!/usr/bin/env python3

import subprocess
import os
import sys
from pprint import pprint
import pandas as pd

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', 'src')))
from run_experiment import run_experiment

DATA_DIR = os.path.join('..', '..', 'data')

DATASETS = ['36-157', '50-218', '75-325', '100-430']
OPTIONS =  ['M', 'N', 'Q']
SEEDS = 10
INSTANCES = 100

COMMAND_OPTIONS = {
    '--mutation-method': 'linear-adaptive',
    '--mutation-probability': '0.7',
    '--crossover': 'uniform',
    '--fitness-alpha': '0.9', 
    '--fitness-beta': '0.4',
    '--elitism': '0.4',
    '--max-generations': '500',
    '--population-size': '-1'   # for autocompute   
}

final_infos = []
for dataset in DATASETS:
    for option in OPTIONS:
        data_dir = os.path.join(DATA_DIR, dataset, '.')
        print(dataset, "*************************************************************8")

        output_dir = os.path.join("results", option)
        os.makedirs(output_dir, exist_ok=True)


        output_file = os.path.join(output_dir, f"result.csv")
        pprint(COMMAND_OPTIONS)
        run_experiment(data_dir, 123, SEEDS, INSTANCES, option, COMMAND_OPTIONS, output_file)

        df = pd.read_csv(output_file)
        final_infos.append({
            'Dataset': f'{dataset}-{option}',
            'Satisfied clauses': f"{df['satisfied_clauses'].mean():.4f} +- {df['satisfied_clauses'].std():.4f}",
            'Ratio satisfied clauses': f"{df['ratio_fully_satisfied'].mean():.4f}",
            'Relative Error': f"{df['relative_error_avg'].mean():.4f} +- {df['relative_error_avg'].std():.4f}",
        })

        print("*************************************************************8")

df = pd.DataFrame(final_infos)
df.to_csv('results.csv')
print(df.to_string())