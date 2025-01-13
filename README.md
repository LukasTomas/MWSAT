# MWSAT

The **Maximum Weighted SAT Problem** is an optimization version of the Boolean satisfiability problem (SAT). In this problem, we are given a Boolean formula, typically in Conjunctive Normal Form (CNF), along with a weight assigned to each clause. The goal is to determine a truth assignment for the variables that **maximizes the total weight of satisfied clauses**.

This repository applies the genetic algorithm to this problem.

```
Usage: ./ga_solve [--help] [--version] [--seed VAR] [--max-generations VAR] [--population-size VAR]
                [--best-individuals-ratio VAR] [--mutation-probability VAR] [--crossover VAR] cnf_file

Genetic algorithm for solving MWSAT problem.

Positional arguments:
  cnf_file                  Path to the CNF file.

Optional arguments:
  -h, --help                shows help message and exits
  -v, --version             prints version information and exits
  -s, --seed                Seed for random number generator. [default: 0]
  --max-generations         Maximum number of generations. [default: 400]
  --population-size         Population size. [default: 100]
  --best-individuals-ratio  Ratio of best individuals to select for the next generation. [default: 0.5]
  --mutation-probability    Probability of mutation. [default: 0.8]
  --crossover               Crossover method. [default: "one-point"]
```