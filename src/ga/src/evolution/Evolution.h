#pragma once

#include "../Cnf.h"
#include "Individual.h"

#include <random>
#include <vector>
#include <map>
#include <iostream>
using namespace std;

class Evolution {
public:

    struct IndividualEvaluation {

        static const double NO_EVALUATION;

        Individual individual;
        double evaluation = NO_EVALUATION;
    };

    struct TrainingData {
        double satisfiedClausesRatio;
        double weightSum;
        double fitness;
    };

    static const map<string, void (*)(const Individual &, const Individual &, Individual &)> crossoverMethods;


    Evolution( const Cnf & cnf, int maxGenerations, int populationSize, double elitismRatio, double mutationRate, const string & crossover, int seed, double fitnessAlpha, double fitnessBeta );

    void run();
    
    const IndividualEvaluation & getBestIndividual();
    int getGeneration() const;
    const vector<TrainingData> & getTrainingData() const;


private:

    const Cnf & cnf;
    const int totalClauses;
    const double totalWeightSum;

    const int maxGenerations;
    const int populationSize;
    
    const int elitismSize;
    const int selectionSize;

    const double mutationProbab;
    const string crossover;

    const double fitnessAlpha;
    const double fitnessBeta;

    int currentGeneration = 0;
    
    vector<IndividualEvaluation> population;
    vector<TrainingData> trainingData;

    mt19937 generator;
    uniform_real_distribution<> distribution = uniform_real_distribution<>(0, 1);


    static int GetElitismSize( int populationSize, double bestIndividualsRatio );
    static int GetSelectionSize( int populationSize, int elitismSize );

    void evaluate();
    void selectBestIndividuals( vector<IndividualEvaluation*> & bestIndividuals, int size );
    void selectRoullete( vector<IndividualEvaluation*> & bestIndividuals );
    double fitness1( double satisfiedClauses, double weightSum ) const;
    double fitness2( double satisfiedClauses, double weightSum ) const;


    void printConfiguration() const;
    void updateTrainingData( const IndividualEvaluation * bestIndividual );


};