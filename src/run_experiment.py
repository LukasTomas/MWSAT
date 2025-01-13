#!/usr/bin/env python3

import os
import glob
import argparse
import pandas as pd
import json

import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

from experiment import Experiment

SCRIPT_NAME = os.path.basename(__file__)
SCRIPT_DESCRIPTION = "Run an experiment with a given directory containing MWSAT instances."

OUTPUT_RESULTS_DIR = 'results'


def load_results(data_directory, results_path):
    with open(results_path, 'r') as f:
        results = f.readlines()
        if len(results) == 0:
            raise Exception(f"Results file is empty: {results_path}")
    
        results_parsed = []
        for result_line in results:
            result_line_split = result_line.split(' ')
            if len(result_line_split) < 3:
                raise Exception(f"Invalid result line: {result_line}")
            
            file_name = f'w{result_line_split[0]}.mwcnf'
            if not os.path.exists(os.path.join(data_directory, file_name)):
                raise Exception(f"Instance file does not exist: {file_name}")
            
            max_weight_sum = int(result_line_split[1])
            results_parsed.append({
                'instance_path': os.path.join(data_directory, file_name),
                'max_weight': max_weight_sum
            })
        
        return results_parsed


def parse_args():
    parser = argparse.ArgumentParser(
        prog=SCRIPT_NAME,
        description=SCRIPT_DESCRIPTION,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )

    parser.add_argument('-d', '--data-dir', type=str, help='Directory containing MWSAT instances.', required=True)
    parser.add_argument('-o', '--option', type=str, choices=['M', 'N', 'Q', 'R'], help='Option to run the experiment.', required=True)
    parser.add_arguemnt('-c', '--command-options-path', type=str, help='Path to the command options file.', required=True)

    # optional
    parser.add_argument('-i', '--instances', type=int, help='Number of instances to run.', default=20, required=False)
    parser.add_argument('-s', '--seed', type=int, help='Random seed.', default=123, required=False)
    parser.add_argument('-n', '--number-seeds', type=int, help='Number of seeds to run.', default=10, required=False)

    args = parser.parse_args()

    if not os.path.exists(args.data_dir):
        parser.error(f"Data directory does not exist: {args.data_dir}")

    return args


def run_experiment(data_dir, seed, number_seeds, n_instances, option, command_options: dict, output_file):
    # get number of variables from directory name
    dir_name = os.path.basename(os.path.dirname(data_dir))
    dir_name_split = dir_name.split('-')
    print(f'Instances: {n_instances}, seeds: {number_seeds}')
    print(f'Variables {dir_name_split[0]}, Clauses {dir_name_split[1]}')
    print(f'--------------------------')

    data_directory = os.path.join(data_dir, option)
    if not os.path.exists(data_directory):
        raise Exception(f"Directory does not exist: {data_directory}")
    
    
    # load results
    results_path = os.path.join(data_dir, f'{option}-opt.dat')
    if not os.path.exists(results_path):
        instances = glob.glob(os.path.join(data_directory, '*.mwcnf'))[:n_instances]
        instances_results = [{'instance_path': instance, 'max_weight': -1} for instance in instances]
    else:
        results = load_results(data_directory, results_path)
        # get max arg instances
        instances_results = results[:n_instances]

    command_options_array = []
    for key, value in command_options.items():
        command_options_array.append(key)
        command_options_array.append(value)

    experiment = Experiment(seed, number_seeds, instances_results, output_file, command_options_array)
    experiment.do_all()


if __name__ == "__main__":
    args = parse_args()
    
    with open(args.command_options_path, 'r') as f:
        command_options = json.load(f)
        run_experiment(args.data_dir, args.seed, args.number_seeds, args.instances, args.option, command_options)