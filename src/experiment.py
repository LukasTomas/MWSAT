import os
import pandas as pd
import numpy as np
from tqdm import tqdm
import subprocess


class Experiment():
    
    EXECUTABLE_NAME = os.path.join(os.path.dirname(__file__), 'ga', 'ga_solve')

    def __init__(self, seed, number_seeds, instances_results, output_file, command_options):
        np.random.seed(seed)
        self._seeds = np.random.randint(0, 10000, number_seeds)

        self._output_file = output_file
        self._instances_results = instances_results

        # check if executable exists
        if not os.path.exists(Experiment.EXECUTABLE_NAME):
            raise Exception(f"Genetic Algorithm executable does not exist: {Experiment.EXECUTABLE_NAME}")

        self._command_options = command_options

    def do_all(self):
        results = []
        satisfied_clauses_ratios_2d = []

        for instance_result in tqdm(self._instances_results):
            satisfied_clauses, ratio_fully_satisfied, weight_sum, generations, fitness_value, relative_error_avg, satisfied_clauses_ratios = self._do_one(instance_result)
            results.append({
                'satisfied_clauses': satisfied_clauses,
                'ratio_fully_satisfied': ratio_fully_satisfied,
                'weight_sum': weight_sum,
                'generations': generations,
                'fitness_value': fitness_value,
                'relative_error_avg': relative_error_avg,
            })

            satisfied_clauses_ratios_2d.append(satisfied_clauses_ratios)

        results_df = pd.DataFrame(results)
        results_df.to_csv(self._output_file, index=False)
        print(results_df.shape)
        print(results_df.describe().to_string())

        # y_avg = np.mean(satisfied_clauses_ratios_2d, axis=0)
        # plt.plot(y_avg)
        # plt.savefig('sat_ratio.png')
        # plt.close()

    def _do_one(self, instance_result):
        satisfied_clauses_array = np.empty(len(self._seeds))
        weight_sum_array = np.empty(len(self._seeds))
        generations_array = np.empty(len(self._seeds))
        relative_error_array = np.empty(len(self._seeds))
        fitness_value_array = np.empty(len(self._seeds))

        for i, seed in enumerate(self._seeds):
            completed_process = subprocess.run([
                Experiment.EXECUTABLE_NAME,
                instance_result['instance_path'],
            ] + self._command_options, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

            if completed_process.returncode != 0:
                print(completed_process.stderr)
                raise Exception(f"Error running Genetic Algorithm on instance: {instance_result['instance_path']}")

            satisfied_clauses, weight_sum, generations, fitness_value, satisfied_clauses_ratios = self._parse_output(completed_process)

            satisfied_clauses_array[i] = satisfied_clauses
            weight_sum_array[i] = weight_sum
            generations_array[i] = generations
            fitness_value_array[i] = fitness_value

            # compute relative error only if max_weight is not -1
            if instance_result['max_weight'] != -1:
                relative_error_array[i] = abs(weight_sum - instance_result['max_weight']) / instance_result['max_weight']
            else:
                relative_error_array[i] = -1

        satifised_clauses_avg = np.mean(satisfied_clauses_array)
        ratio_fully_satisfied = np.sum(satisfied_clauses_array == 1.0) / len(satisfied_clauses_array)

        weight_sum_avg = np.mean(weight_sum_array)
        generations_avg = np.mean(generations_array)
        fitness_value_array = np.mean(fitness_value_array)
        relative_error_avg = np.mean(relative_error_array)

        return satifised_clauses_avg, ratio_fully_satisfied, weight_sum_avg, generations_avg, fitness_value_array, relative_error_avg, []

    def _parse_output(self, completed_process):
        output = completed_process.stdout.split('\n')
        first_line = output[0]
        numbers = first_line.split(' ')
        if len(numbers) != 4:
            raise Exception(f"Invalid output: {first_line}, should contain 4 numbers")
        
        satisfied_clauses = float(numbers[0])
        weight_sum = int(numbers[1])
        generations = int(numbers[2])
        fitness_value = float(numbers[3])

        # parse training data
        satisfied_clauses_ratios = [float(x) for x in output[1].split(' ')[:-1]]
        weight_sums = [int(float(x)) for x in output[2].split(' ')[:-1]]
        fitness = [float(x) for x in output[3].split(' ')[:-1]]

        if len(satisfied_clauses_ratios) != generations or len(weight_sums) != generations or len(fitness) != generations:
            raise Exception(f"Invalid output: {output[1:4]}, should contain {generations} numbers")

        return satisfied_clauses, weight_sum, generations, fitness_value, satisfied_clauses_ratios