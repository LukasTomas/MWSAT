The **Maximum Weighted SAT Problem** is an optimization version of the Boolean satisfiability problem (SAT). In this problem, we are given a Boolean formula, typically in Conjunctive Normal Form (CNF), along with a weight assigned to each clause. The goal is to determine a truth assignment for the variables that **maximizes the total weight of satisfied clauses**.

This repository applies the genetic algorithm to this problem.

```
.Usage: ./ga_solve [--help] [--version] [--seed VAR] [--max-generations VAR] [--population-size VAR] [--elitism VAR] 
                   [--mutation-probability VAR] [--mutation-method VAR] [--fitness-alpha VAR] [--fitness-beta VAR] 
                   [--crossover VAR] cnf_file

Genetic algorithm for solving MWSAT problem.

Positional arguments:
  cnf_file                Path to the CNF file.

Optional arguments:
  -h, --help              shows help message and exits
  -v, --version           prints version information and exits
  -s, --seed              Seed for random number generator. [nargs=0..1] [default: 0]
  --max-generations       Maximum number of generations. [nargs=0..1] [default: 400]
  --population-size       Population size. If population size is set -1, it will be computed adaptively. [nargs=0..1] [default: 100]
  --elitism               Ratio of best individuals to select for the next generation. [nargs=0..1] [default: 0.5]
  --mutation-probability  Probability of mutation. [nargs=0..1] [default: 0.8]
  --mutation-method       Mutation method, possible values: fixed linear-adaptive  [nargs=0..1] [default: "fixed"]
  --fitness-alpha         Alpha parameter for fitness function. [nargs=0..1] [default: 0.5]
  --fitness-beta          Beta parameter for fitness function. [nargs=0..1] [default: -1]
  --crossover             Crossover method, possible values: one-point two-point uniform  [nargs=0..1] [default: "one-point"]
```
