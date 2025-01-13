#include "Evolution.h"

#include <algorithm>


// PUBLIC METHODS -------------------------------------------------------------

const map<string, void (*)(const Individual &, const Individual &, Individual &)> Evolution::crossoverMethods = {
    {"one-point", Individual::onePointCrossover},
    {"two-point", Individual::twoPointCrossover},
    {"uniform", Individual::uniformCrossover}
};


Evolution::Evolution( const Cnf & cnf, int maxGenerations, int populationSize, double elitismRatio, double mutationProbab, const string & crossover, int seed, double fitnessAlpha, double fitnessBeta ) 
    :   cnf(cnf), totalClauses(cnf.getClauses().size()), totalWeightSum(cnf.getTotalWeightSum()),
        maxGenerations(maxGenerations), populationSize(populationSize), 
        elitismSize(Evolution::GetElitismSize(populationSize, elitismRatio)), selectionSize(Evolution::GetSelectionSize(populationSize, elitismSize)),
        mutationProbab(mutationProbab), crossover(crossover),
        generator(seed),
        fitnessAlpha(fitnessAlpha), fitnessBeta(fitnessBeta) {
    
    printConfiguration();

    void (*crossoverFunction)(const Individual & first, const Individual & second, Individual & offspring);
    try {
        crossoverFunction = crossoverMethods.at(crossover);
    } catch ( const out_of_range & e ) {
        throw invalid_argument("Invalid crossover method: " + crossover);
    }

    Individual::SetSettings({
        cnf.getVariables(),
        mutationProbab,
        crossoverFunction
    });

    // Initialize first generation with random values
    for( int i = 0; i < populationSize; i++ ) {
        population.push_back({Individual(true), IndividualEvaluation::NO_EVALUATION});        
        // cout << population[i].individual << "\t eval:" << population[i].evaluation << endl;
    }
}

void Evolution::run() {
    vector<IndividualEvaluation*> selectedIndividuals/*(bestIndividualsSize, nullptr)*/;

    while( currentGeneration < maxGenerations ) {
        // 1) evaluate
        evaluate();                

        vector<IndividualEvaluation> newPopulation;
        // 2) elitism
        selectBestIndividuals(selectedIndividuals, elitismSize);
        updateTrainingData(selectedIndividuals[0]);
        for ( int i = 0; i < elitismSize; i++ )
            newPopulation.push_back(*selectedIndividuals[i]);

        // 3) select k parents
        selectedIndividuals.clear();
        selectRoullete(selectedIndividuals);

        // // 3) crossover with mutation - every new individual is mutated
        for( int i = 0; i < selectionSize; i++ ) {
            if ( newPopulation.size() >= populationSize )
                break;

            for( int j = i+1; j < selectionSize; j++ ) {
                if ( newPopulation.size() >= populationSize )
                    break;

                newPopulation.push_back(
                    {selectedIndividuals[i]->individual.crossoverAndMutate(selectedIndividuals[j]->individual), 
                    IndividualEvaluation::NO_EVALUATION}
                );
            }
        }

        // 4) mutate

        // population should be full
        if ( newPopulation.size() < populationSize )
            throw runtime_error("Population size is not full");

        population.swap(newPopulation);
        
        selectedIndividuals.clear();
        currentGeneration++;
    }

    evaluate();
}

const Evolution::IndividualEvaluation & Evolution::getBestIndividual() {
    vector<IndividualEvaluation*> bestIndividuals;
    selectBestIndividuals(bestIndividuals, 1);
    return *bestIndividuals[0]; 
}

int Evolution::getGeneration() const {
    return currentGeneration;
}

const vector<Evolution::TrainingData> & Evolution::getTrainingData() const {
    return trainingData;
}


// PRIVATE METHODS -------------------------------------------------------------

int Evolution::GetElitismSize( int populationSize, double elitismRatio ) {
    if ( elitismRatio < 0 || elitismRatio > 1 )
        throw invalid_argument("Ratio of best individuals must be between 0 and 1");

    int elitismSize = populationSize * elitismRatio;
    if ( elitismSize < 2 )
        throw invalid_argument("Ratio of best individuals is too small -> best individuals size must be at least 2");
    if ( elitismSize > populationSize )
        throw invalid_argument("Ratio of best individuals is too big -> best individuals size must be smaller than population size");

    return elitismSize;
}

int Evolution::GetSelectionSize( int populationSize, int elitismSize ) {
    /*
     * binomial coefficient(k, 2) >= n  =>  k(k-1)/2 >= n
     * k^2 - k - 2n >= 0
     * k = (1 + sqrt(1 + 8n)) / 2
     * k = ceil(k)
     * 
    */
    int children = populationSize - elitismSize;
    double sqrtValue = sqrt(1 + 8*children);
    int parentSize = ceil((1 + sqrtValue) / 2);
    return parentSize;
}

// 1) Evaluation ----------
void Evolution::evaluate() {
    for( size_t i = 0; i < population.size(); i++ ) {
        IndividualEvaluation & individualEval = population[i];
        // Skip if individual has already been evaluated in the previous generation
        // This can happen if the individual was one of the best individuals in the previous generation
        if ( individualEval.evaluation != IndividualEvaluation::NO_EVALUATION ) {
            // cout << individualEval.evaluation << " ";
            continue;
        }

        int satisfiedClauses = cnf.getSatisfiedClauses(individualEval.individual.getChromosome());
        double weightSum = cnf.getWeightSum(individualEval.individual.getChromosome());
        if ( fitnessBeta != -1 )
            individualEval.evaluation = fitness2(satisfiedClauses, weightSum);
        else
            individualEval.evaluation = fitness1(satisfiedClauses, weightSum);

        // cout << individualEval.evaluation << " ";
    }
    // cout << endl;
}

double Evolution::fitness1(double satisfiedClauses, double weightSum) const {
    double satisfiedRatio = satisfiedClauses / totalClauses;
    double weightRatio = weightSum / totalWeightSum;

    // Combine the two metrics with adjustable weighting factors
    const double alpha = fitnessAlpha; // Emphasis on clause satisfaction
    const double beta = 1 - alpha;    // Emphasis on weight satisfaction

    return alpha * satisfiedRatio + beta * weightRatio;
}

double Evolution::fitness2(double satisfiedClauses, double weightSum) const {
    // Ratio of satisfied clauses
    double satisfiedRatio = satisfiedClauses / totalClauses;

    // Ratio of satisfied weights
    double weightRatio = weightSum / totalWeightSum;

    // Penalty for unsatisfied clauses (encourages solutions that satisfy all clauses)
    double unsatisfiedPenalty = (totalClauses - satisfiedClauses) / totalClauses;

    // Scaling factors to balance satisfaction and weight contributions
    const double alpha = fitnessAlpha;
    const double beta = 1 - alpha;
    const double gamma = fitnessBeta; // Weight for penalty

    // Final fitness score
    return alpha * satisfiedRatio + beta * weightRatio - gamma * unsatisfiedPenalty;
}

// ------------------------

// 2) Selection -----------
void Evolution::selectBestIndividuals( vector<IndividualEvaluation*>& bestIndividuals, int size ) {
    auto comparator = [](const Evolution::IndividualEvaluation* a, const Evolution::IndividualEvaluation* b) {
        return a->evaluation > b->evaluation; // Compare based on evaluation
    };

    int index = 0;

    // Insert the first few elements directly
    for (; index < size; index++) {
        IndividualEvaluation* individual = &population[index];
        // Find the position to insert using the same comparator
        auto it = std::lower_bound(bestIndividuals.begin(), bestIndividuals.end(), individual, comparator);
        bestIndividuals.insert(it, individual);
    }

    // Iterate over the remaining population
    for (; index < populationSize; index++) {
        IndividualEvaluation* individual = &population[index];

        // Check if the current individual is better than the worst in the bestIndividuals vector
        if (comparator(individual, bestIndividuals.back())) {
            // Replace the worst individual
            bestIndividuals.pop_back(); // Remove the worst
            auto it = std::lower_bound(bestIndividuals.begin(), bestIndividuals.end(), individual, comparator);
            bestIndividuals.insert(it, individual);
        }
    }
}

void Evolution::selectRoullete( vector<IndividualEvaluation*> & selectedIndividuals ) {
    for ( int i = 0; i < selectionSize; i++ ) {
        /* Roullete Wheel Selection (Fitness proportionate selection) */
        
        // 1) Compute proportionate fitness - probability of individual to be selected
        double totalFitness = 0;
        for ( const IndividualEvaluation & individualEval : population )
            totalFitness += individualEval.evaluation;

        // 2) Generate random probability
        double randomProbability = distribution(generator);
        
        // 3) Select individuals - using binary search
        int selectedIndex = -1;

        double currentComulativeProbability = 0.0;
        for ( int i = 0; i < populationSize; i++ ) {
            double probability = population[i].evaluation / totalFitness;
            currentComulativeProbability += probability;

            if ( randomProbability <= currentComulativeProbability ) {
                selectedIndex = i;
                break;
            }
        }

        // Falbback, though it shouldn't happen due to the nature of the algorithm
        if ( selectedIndex == -1 )
            selectedIndex = populationSize - 1;

        selectedIndividuals.push_back(&population[selectedIndex]);
    }
}
// ------------------------

void Evolution::printConfiguration() const {
    cerr << "Configuration: " << endl;
    cerr << "\tMax generations: " << maxGenerations << endl;
    cerr << "\tPopulation size: " << populationSize << endl;
    cerr << "\tElitism size: " << elitismSize << endl;
    cerr << "\tSelection size: " << selectionSize << endl;
    cerr << endl;
    cerr << "\tMutation probability: " << mutationProbab << endl;
    cerr << "---------------------------------" << endl;
}

void Evolution::updateTrainingData( const IndividualEvaluation * bestIndividual ) {
    int satisfiedClauses = cnf.getSatisfiedClauses(bestIndividual->individual.getChromosome());
    double satisfiedClausesRatio = (double)satisfiedClauses / totalClauses;

    double weightSum = cnf.getWeightSum(bestIndividual->individual.getChromosome());
    
    double fitness = bestIndividual->evaluation;


    trainingData.push_back({satisfiedClausesRatio, weightSum, fitness});
}


// INNER CLASSES -------------------------------------------------------------

const double Evolution::IndividualEvaluation::NO_EVALUATION = -1;
