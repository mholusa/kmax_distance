/*============================================================================*/
/* kmax_distance.h                                                        */
/*                                                        mrl vsb-tuo 2014-17 */
/*============================================================================*/

#ifndef _KMAX_H_
#define _KMAX_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <vector>
#include "distribfunction.h"
#include "avl_tree.h"
#include "image.h"


#define MAX_GEO_DIST     1.0e8
#define GEO_MAX_EPSX     1.0e-8
#define MAX_NUM_MAXS           40
#define MAX_NUM_TO_OVERWRITE  2000
#define MAX_NUM_OF_SEEDS  6000

typedef struct { void *next; void *back; int isUsed; int wasMin; NodeAVL *qPtr; int imgIdx; double sumVals; int numVals; double vals[1]; } MaxRec;
//______________________________________________________________________________

class ProbabEstimator {

  public:
    ProbabEstimator( const char *fileName, const int numMaxs );
    ~ProbabEstimator( void );
    double getProbab( const double *vecMaxs, const int numReplaced, const int pathLength );
    double getProbabFast( const double *vecMaxs, const int numReplaced, const int pathLength );
    double getMaxProbab( const double *vecMaxs, const int numReplaced, const int minPathLength, const int maxPathLength, const int pathLengthStep );

  private:
    double getProbabGt0( const double minValInRetained, const int numReplaced, const int pathLength );
    double getProbabGt( const double minValInRetained, const int numReplaced, const int pathLength );
    double getProbabLt( const double maxValInReplaced, const int numReplaced, const int pathLength );
    double getProbabGtFast1( const double minValInRetained, const int numReplaced, const int pathLength );
    double getProbabGtFast2( const double minValInRetained, const int numReplaced, const int pathLength );
    double getProbabLtFast1( const double maxValInReplaced, const int numReplaced, const int pathLength );
    double getProbabLtFast2( const double maxValInReplaced, const int numReplaced, const int pathLength );
	
	double combNumber( const int n, const int k );
	double pown(double base, int exp);
	
    int numMaxs;
    int maxPathLength;
    DistribFunction *distribFunction;

};

class KMaxDistance {
	
	public:
        
        double compute( const Image<unsigned char>& inputImage, const int numDistMaxs, const int* srcPixs, const int numSrcPixs, const int dstPix,
    Image<double> *distImg = NULL, std::vector<int>* backPtrs = NULL, std::vector<double>* maxVector = NULL, const double sigmaForWeight = 1.0/3.0, const double baseCost = 0.001);

        double compute( const double *xEdgeCosts, const double *yEdgeCosts, const int width, const int height, const int numDistMaxs,
    const int *srcPixs, const int numSrcPixs, const int dstPix, double *dists = NULL, int* backPtrs = NULL, double *maxVector = NULL );
        
        void edgeCosts( const double *image, const int width, const int height, const double brSigma, const double sxy, double *xCosts, double *yCosts );
        
        inline void setProbabThresh(const double value) { probabThresh = value; }
        inline double getProbabThresh() { return probabThresh; }
        
	private:
		int getMaxsRecSize( const int numVals );
		double sumVals( const double *vals, const int numVals );
		void printVals( const double *vals, const int numVals, const int maxNumVals );
		inline double gaussInv( const double brDiff, const double sigma ){ return 1.0 - exp(-0.5*(brDiff*brDiff)/(sigma*sigma)); }
		void printRec( const char *heading, MaxRec *rec, const int numDistMaxs );
		void printRecs( const char *heading, MaxRec *srcRec, const int numDistMaxs );
		MaxRec* createNewMaxRec( const int imgIdx, const int numVals );
		void copyVals( const MaxRec *src, MaxRec *dst );
		int putValToItsSortedPlace( const double val, double *vals, const int numVals );
		void modifyDstMaxs( MaxRec *dst, const double edgeCost );
		void analyseVecsOfMaxs( const double *vals1, const double *vals2, const int numVals, int *isBetterCnt1, int *isBetterCnt2 );
		int compareTwoVecsOfMaxs( MaxRec *rec1, MaxRec *rec2 );
		int compareVecOfMaxsWithAllVecsInNode( MaxRec *newRec, const MaxRec *nodeRec, MaxRec **recsToOverWrite, int *numToOverwrite );
		void addFirstPathToNodeAVL( MaxRec *srcRec, const int idxOfDstNode, const double edgeCost, const int numDistMaxs, MaxRec **maxs, NodeAVL **queue );
		int getNumMaxRecs( MaxRec *srcRec );
		void freeMaxRecs( MaxRec *rec );
		void addFurtherPathToNodeAVL( MaxRec *srcRec, const int idxOfDstPix, const double edgeCost, const int numDistMaxs, MaxRec **maxs, NodeAVL **queue );
		void addPathToNodeAVL( MaxRec *srcRec, const int dstNodeIdx, const double edgeCost, const int numDistMaxs, MaxRec **maxs, NodeAVL **queue );
		
        

		int numMaxRecs, numMaxRecsInOneNode;
		//double minValFromTree;
		ProbabEstimator *probabEstimator;
		double probabThresh = -0.1;
		int numDistMaxsGl;
        
};
/*============================================================================*/
#endif

