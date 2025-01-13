#include "Cnf.h"
#include "evolution/Evolution.h"

#include "argparse.hpp"

#include <iostream>
#include <vector>
#include <ctime>
using namespace std;

int main( int argc, char* argv[] ) {
    argparse::ArgumentParser parser(argv[0]);
    parser.add_description("Genetic algorithm for solving MWSAT problem.");

    parser.add_argument("cnf_file")
        .help("Path to the CNF file.");
    parser.add_argument("-s", "--seed")
        .help("Seed for random number generator.")
        .default_value(0)
        .action([](const string & value) { return stoi(value); });
    // genetic algorithm arguments
    parser.add_argument("--max-generations")
        .help("Maximum number of generations.")
        .default_value(400)
        .action([](const string & value) {return stoi(value); });
    parser.add_argument("--population-size")
        .help("Population size.")
        .default_value(100)
        .action([](const string & value) { return stoi(value); });
    parser.add_argument("--elitism")
        .help("Ratio of best individuals to select for the next generation.")
        .default_value(0.5)
        .action([](const string & value) { return stod(value); });
    parser.add_argument("--mutation-probability")
        .help("Probability of mutation.")
        .default_value(0.8)
        .action([](const string & value) { return stod(value); });

    // fintess function arguments
    parser.add_argument("--fitness-alpha")
        .help("Alpha parameter for fitness function.")
        .default_value(0.5)
        .action([](const string & value) { return stod(value); });
    parser.add_argument("--fitness-beta")
        .help("Beta parameter for fitness function.")
        .default_value(-1.0)
        .action([](const string & value) { return stod(value); });

    vector<string> crossoverMethods = {"one-point", "two-point", "uniform"};
    string crossoverMethodsStr = "";
    for ( const string & method : crossoverMethods )
        crossoverMethodsStr += method + " ";
    parser.add_argument("--crossover")
        .help("Crossover method, possible values: " + crossoverMethodsStr)
        .default_value("one-point")
        .action([crossoverMethods](const string & value) {
            if ( find(crossoverMethods.begin(), crossoverMethods.end(), value) == crossoverMethods.end() )
                throw invalid_argument("Invalid crossover method");
            return value;
        });

    try {
        parser.parse_args(argc, argv);
    } catch ( const exception & e ) {
        cerr << e.what() << endl;
        return 1;
    }

    string filepath = parser.get<string>("cnf_file");
    int seed = parser.get<int>("--seed");

    int maxGenerations = parser.get<int>("--max-generations");
    int populationSize = parser.get<int>("--population-size");
    double elitism = parser.get<double>("--elitism");
    double mutationProbability = parser.get<double>("--mutation-probability");
    string crossover = parser.get<string>("--crossover");
    double fitnessAlpha = parser.get<double>("--fitness-alpha");
    double fitnessBeta = parser.get<double>("--fitness-beta");

    srand(seed);

    Cnf cnf(filepath);
    Evolution evolution(
        cnf,
        maxGenerations,      // max generations
        populationSize,       // population size
        elitism,        // ratio of best individuals to select
        mutationProbability,     // mutation probability
        crossover,      // crossover method
        seed,
        fitnessAlpha,
        fitnessBeta
    );

    evolution.run();

    // output info in format:
    // satisfied_clauses weight_sum generations
    const Evolution::IndividualEvaluation & bestIndividual = evolution.getBestIndividual();
    int satisfiedClauses = cnf.getSatisfiedClauses(bestIndividual.individual.getChromosome());
    double satisfiedClausesRatio = (double)satisfiedClauses / cnf.getClauses().size();
    
    cout << satisfiedClausesRatio << " "
         << cnf.getWeightSum(bestIndividual.individual.getChromosome()) << " "
         << evolution.getGeneration() << " "
         << bestIndividual.evaluation << endl;

    const vector<Evolution::TrainingData> & trainingData = evolution.getTrainingData();
    string satisfiedClausesRatios = "";
    string weightSums = "";
    string fitness = "";
    for ( const Evolution::TrainingData & data : trainingData ) {
        satisfiedClausesRatios += to_string(data.satisfiedClausesRatio) + " ";
        weightSums += to_string(data.weightSum) + " ";
        fitness += to_string(data.fitness) + " ";
    }
    cout << satisfiedClausesRatios << endl;
    cout << weightSums << endl;
    cout << fitness << endl;

    return 0;
}