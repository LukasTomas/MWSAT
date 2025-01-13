#include "crossovers.h"


void crossovers::onePointCrossover( const Individual & first, const Individual & second, Individual & offspring ) {
    int middleIndex = Individual::IndividualSettings.chromosomeSize / 2;
    for( int i = 0; i < middleIndex; i++ )
        offspring.chromosome[i] = this->chromosome[i];
    for( int i = middleIndex; i < Individual::IndividualSettings.chromosomeSize; i++ )
        offspring.chromosome[i] = second.chromosome[i];
}

void crossovers::twoPointCrossover( const Individual & first, const Individual & second, Individual & offspring ) {

}

void crossovers::uniformCrossover( const Individual & first, const Individual & second, Individual & offspring ) {

}
