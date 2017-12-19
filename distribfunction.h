/*============================================================================*/
/* distribfunction.h                                                              */
/*============================================================================*/

#ifndef _DISTRIB_FUNCTION_H_
#define _DISTRIB_FUNCTION_H_

class DistribFunction {

  public:

    DistribFunction( const char *fileName );
    ~DistribFunction( void );
    double getDistrib( const double x );
    double getDistribSumSquares( void );

  private:

    double *distrib;
    double minValExp, maxValExp;
    int numHistIntervals;
};

/*============================================================================*/
#endif

