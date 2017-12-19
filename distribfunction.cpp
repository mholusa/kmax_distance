/*============================================================================*/
/* distribfunction.cpp                                                            */
/*============================================================================*/

#include <stdlib.h>
#include <stdio.h>

#include "distribfunction.h"

DistribFunction::DistribFunction( const char *fileName ){
    FILE *inpFile = fopen( fileName, "rb" );
    if ( inpFile == NULL ) { fprintf(stderr, "Probability distribution file not found\n"); exit(-1); /*return;*/ }
    fread( &numHistIntervals, sizeof(numHistIntervals), 1, inpFile );
    fread( &minValExp, sizeof(minValExp), 1, inpFile );
    fread( &maxValExp, sizeof(maxValExp), 1, inpFile );
    distrib = (double*)malloc( numHistIntervals*sizeof(distrib[0]));
    fread( distrib, sizeof(distrib[0]), numHistIntervals, inpFile );
    fclose( inpFile );
}

DistribFunction::~DistribFunction( void ){
    free( distrib );
}

double DistribFunction::getDistrib( const double x ){
    if ( x <= minValExp ) return 0.0;
    if ( x >= maxValExp ) return 1.0;
    double intervalLenght = (maxValExp - minValExp)/(double)numHistIntervals;
    int idx = (int)((x-minValExp)/intervalLenght);
    return distrib[idx];
}

double DistribFunction::getDistribSumSquares( void ){
    double sumSquares = 0.0;
    for ( int i = 0; i < numHistIntervals; i++ ){
        sumSquares += distrib[i]*distrib[i];
    }
    return sumSquares; 
}
/*============================================================================*/
