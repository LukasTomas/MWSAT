#include "Cnf.h"
#include "evolution/Evolution.h"

#include "argparse.hpp"

#include <iostream>
#include <vector>
#include <ctime>
#include <map>
#include <cmath>
using namespace std;


void output_results( Cnf & cnf, Evolution & evolution ) {
    // output info in format:
    // satisfied_clauses weight_sum generations best_individual_evaluation
    const Evolution::IndividualEvaluation & bestIndividual = evolution.getBestIndividual();
    int satisfiedClauses = cnf.getSatisfiedClauses(bestIndividual.individual.getChromosome());
    double satisfiedClausesRatio = (double)satisfiedClauses / cnf.getClauses().size();
    
    cout << satisfiedClausesRatio << " "
         << cnf.getWeightSum(bestIndividual.individual.getChromosome()) << " "
         << evolution.getGeneration() << " "
         << bestIndividual.evaluation << endl;


    // output progress 
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
}

int computePopuluationsize( int variables ) {
    const double a = 0.005737;
    const double b = 2.3115;
    const int optimalPopulationSize = round(a * pow(variables, b));

    if ( optimalPopulationSize < 2 )
        return 2;
    return optimalPopulationSize;
}


map<string, MutationMethod> mutationMethods = {
    {"fixed", MutationMethod::FIXED},
    {"linear-adaptive", MutationMethod::LINEAR_ADAPTIVE}
};

map<string, CrossoverMethod> crossoverMethods = {
    {"one-point", CrossoverMethod::ONE_POINT},
    {"two-point", CrossoverMethod::TWO_POINT},
    {"uniform", CrossoverMethod::UNIFORM}
};

static const int POPULATION_SIZE_ADAPTIVE = -1;

void parseArguments( int argc, char* argv[], argparse::ArgumentParser & parser ) {
    parser.add_argument("cnf_file")
        .help("Path to the CNF file.");
    parser.add_argument("-s", "--seed")
        .help("Seed for random number generator.")
        .default_value(0)
        .action([](const string & value) { return stoi(value); });

    // Genetic algorithm arguments -----------------------------------------
    parser.add_argument("--max-generations")
        .help("Maximum number of generations.")
        .default_value(400)
        .action([](const string & value) {return stoi(value); });
    parser.add_argument("--population-size")
        .help("Population size. If population size is set -1, it will be computed adaptively.")
        .default_value(100)
        .action([](const string & value) { return stoi(value); });
    parser.add_argument("--elitism")
        .help("Ratio of best individuals to select for the next generation.")
        .default_value(0.5)
        .action([](const string & value) { return stod(value); });

    // Mutation arguments
    parser.add_argument("--mutation-probability")
        .help("Probability of mutation.")
        .default_value(0.8)
        .action([](const string & value) { return stod(value); });

    // check just to be sure, if all methods are covered
    if ( mutationMethods.size() != MutationMethod::NUMBER_OF_MUTATION_METHODS )
        throw invalid_argument("Not all mutation methods are covered in the parser.");
    string mutationMethodsStr = "";
    for ( const auto & method : mutationMethods )
        mutationMethodsStr += method.first + " ";
    parser.add_argument("--mutation-method")
        .help("Mutation method, possible values: " + mutationMethodsStr)
        .default_value("fixed")
        .action([](const string & value) {
            if ( mutationMethods.find(value) == mutationMethods.end() )
                throw invalid_argument("Invalid mutation method.");
            return value;
        });

    // Fintess function arguments
    parser.add_argument("--fitness-alpha")
        .help("Alpha parameter for fitness function.")
        .default_value(0.5)
        .action([](const string & value) { return stod(value); });
    parser.add_argument("--fitness-beta")
        .help("Beta parameter for fitness function.")
        .default_value(-1.0)
        .action([](const string & value) { return stod(value); });

    // Crossover arguments
    // check just to be sure, if all methods are covered
    if ( crossoverMethods.size() != CrossoverMethod::NUMBER_OF_CROSSOVER_METHODS )
        throw invalid_argument("Not all crossover methods are covered in the parser.");
    string crossoverMethodsStr = "";
    for ( const auto & method : crossoverMethods )
        crossoverMethodsStr += method.first + " ";
    parser.add_argument("--crossover")
        .help("Crossover method, possible values: " + crossoverMethodsStr)
        .default_value("one-point")
        .action([](const string & value) {
            if ( crossoverMethods.find(value) == crossoverMethods.end() )
                throw invalid_argument("Invalid crossover method.");
            return value;
        });

    parser.parse_args(argc, argv);
}

int main( int argc, char* argv[] ) {
    argparse::ArgumentParser parser(argv[0]);
    parser.add_description("Genetic algorithm for solving MWSAT problem.");
    try {
        parseArguments(argc, argv, parser);
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
    MutationMethod mutationMethod = mutationMethods.at(parser.get<string>("--mutation-method"));
    CrossoverMethod crossover = crossoverMethods.at(parser.get<string>("--crossover"));
    double fitnessAlpha = parser.get<double>("--fitness-alpha");
    double fitnessBeta = parser.get<double>("--fitness-beta");

    srand(seed);

    Cnf cnf(filepath);

    if ( populationSize == POPULATION_SIZE_ADAPTIVE )
        populationSize = computePopuluationsize(cnf.getVariables());

    Evolution evolution(
        cnf,
        maxGenerations,      // max generations
        populationSize,       // population size
        elitism,        // ratio of best individuals to select
        
        mutationProbability,     // mutation probability
        mutationMethod,     // mutation method

        crossover,      // crossover method
        seed,
        fitnessAlpha,
        fitnessBeta
    );

    evolution.run();

    output_results(cnf, evolution);
    return 0;
}