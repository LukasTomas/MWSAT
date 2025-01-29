#pragma once

#include <vector>
#include <iostream>
using namespace std;

class Individual {
public:

    struct Settings {
        static const int NO_CHROMOZONE_SIZE;

        int chromosomeSize;
        void (*crossover)(const Individual & first, const Individual & second, Individual & offspring);
    };

    static const Settings & GetSettings();
    static void SetSettings( const Settings & settings );

    static void onePointCrossover( const Individual & first, const Individual & second, Individual & offspring );
    static void twoPointCrossover( const Individual & first, const Individual & second, Individual & offspring );
    static void uniformCrossover( const Individual & first, const Individual & second, Individual & offspring );


    Individual( bool randomize = false );

    Individual crossoverAndMutate( const Individual & second, double mutationProbab );

    const vector<bool> & getChromosome() const;

    friend ostream & operator<<( ostream & os, const Individual & individual );

private:

    static Settings IndividualSettings;

    vector<bool> chromosome;

};