#include "Individual.h"

#include <stdexcept>

// STATIC PUBLIC METHODS -------------------------------------------------------------

const int Individual::Settings::NO_CHROMOZONE_SIZE = -1;

Individual::Settings Individual::IndividualSettings = {
    Individual::Settings::NO_CHROMOZONE_SIZE,
    Individual::onePointCrossover
};


const Individual::Settings & Individual::GetSettings() {
    return Individual::IndividualSettings;
}

void Individual::SetSettings( const Individual::Settings & settings ) {
    Individual::IndividualSettings = settings;
}


void Individual::onePointCrossover( const Individual & first, const Individual & second, Individual & offspring ) {
    int middleIndex = Individual::IndividualSettings.chromosomeSize / 2;
    for( int i = 0; i < middleIndex; i++ )
        offspring.chromosome[i] = first.chromosome[i];
    for( int i = middleIndex; i < Individual::IndividualSettings.chromosomeSize; i++ )
        offspring.chromosome[i] = second.chromosome[i];

}

void Individual::twoPointCrossover( const Individual & first, const Individual & second, Individual & offspring ) {
    int firstMiddleIndex = Individual::IndividualSettings.chromosomeSize / 3;
    int secondMiddleIndex = 2 * Individual::IndividualSettings.chromosomeSize / 3;
    for( int i = 0; i < firstMiddleIndex; i++ )
        offspring.chromosome[i] = first.chromosome[i];
    for( int i = firstMiddleIndex; i < secondMiddleIndex; i++ )
        offspring.chromosome[i] = second.chromosome[i];
    for( int i = secondMiddleIndex; i < Individual::IndividualSettings.chromosomeSize; i++ )
        offspring.chromosome[i] = first.chromosome[i];
}

void Individual::uniformCrossover( const Individual & first, const Individual & second, Individual & offspring ) {
    for ( int i = 0; i < Individual::IndividualSettings.chromosomeSize; i++ ) {
        int random = rand() % 2;
        offspring.chromosome[i] = (random == 0) ? first.chromosome[i] : second.chromosome[i];
    }
}


// PUBLIC METHODS -------------------------------------------------------------

Individual::Individual( bool randomize ) {
    if( Individual::IndividualSettings.chromosomeSize == Individual::Settings::NO_CHROMOZONE_SIZE )
        throw logic_error("Chromosome size not set");

    chromosome.resize(Individual::IndividualSettings.chromosomeSize, 0);
    if( randomize )
        for( int i = 0; i < Individual::IndividualSettings.chromosomeSize; i++ )
            chromosome[i] = rand() % 2;
}

Individual Individual::crossoverAndMutate( const Individual & second, double mutationProbab ) {
    Individual offspring;

    // crossover
    Individual::IndividualSettings.crossover(*this, second, offspring);

    // mutate
    for ( int i = 0; i < Individual::IndividualSettings.chromosomeSize; i++ ) {
        int random = rand() % 100;
        if ( random < mutationProbab*100 )
            offspring.chromosome[i] = !offspring.chromosome[i];
    }

    return offspring;
}

const vector<bool> & Individual::getChromosome() const {
    return this->chromosome;
}

ostream & operator<<( ostream & os, const Individual & individual ) {
    for( int i = 0; i < Individual::IndividualSettings.chromosomeSize; i++ )
        os << individual.chromosome[i];
    return os;
}