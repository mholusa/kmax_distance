/*============================================================================*/
/* kmax_distance.cpp                                                          */
/*                                                                            */
/* Computing kmax distance                                mrl vsb-tuo 2014-17 */ 
/*============================================================================*/

#include "kmax_distance.h"

//______________________________________________________________________________

#define nCHECK_TREE
#define nCHECK_NODES
#define nCHECK_FAST_PROBABS
#define nDEBUG_MODE

//______________________________________________________________________________


#ifdef DEBUG_MODE
double meanNumRecs;
int maxNumRecsInNode;
#endif


int KMaxDistance::getMaxsRecSize( const int numVals ){
    MaxRec *rec; return sizeof(*rec) + (numVals-1)*sizeof(rec->vals[0]);
}
//______________________________________________________________________________

double KMaxDistance::sumVals( const double *vals, const int numVals ){
    double sumVals = 0.0;
    for ( int i = 0; i < numVals; ++i ) sumVals += vals[i];
    return sumVals;
} 
//______________________________________________________________________________

void KMaxDistance::printVals( const double *vals, const int numVals, const int maxNumVals ){
    printf( "vals=( " );
    for ( int i = 0; i < numVals; ++i ) printf( "%.3f ", vals[i] );
    for ( int i = numVals; i < maxNumVals; ++i ) printf( "0.000 " );
    printf( ")" );
}

void KMaxDistance::printRec( const char *heading, MaxRec *rec, const int numDistMaxs ){
    printf( "%s ", heading );
    if ( rec->isUsed ){
        printVals( rec->vals, rec->numVals, numDistMaxs );
        double sum = sumVals( rec->vals, rec->numVals );
        printf( " sum=%.3f (%.3f) isUsed=%d wasMin=%d imgIdx=%3d\n", sum, rec->sumVals, rec->isUsed, rec->wasMin, rec->imgIdx );
    }
}

void KMaxDistance::printRecs( const char *heading, MaxRec *srcRec, const int numDistMaxs ){
    MaxRec *rec = srcRec;
    while ( rec != NULL ){
        printRec( heading, rec, numDistMaxs );
        rec = (MaxRec*)rec->next;
    }
}

MaxRec* KMaxDistance::createNewMaxRec( const int imgIdx, const int numVals ){
    MaxRec *rec = (MaxRec*)malloc( getMaxsRecSize( numVals ));
    rec->next = NULL;
    rec->isUsed = 1;
    //rec->prev = NULL;
    rec->back = NULL;
    rec->wasMin = 0;
    rec->imgIdx = imgIdx;
    rec->sumVals = 0.0;
    rec->numVals = numVals;
    for ( int i = 0; i < numVals; ++i ) rec->vals[i] = 0.0;
    numMaxRecs++;
    return rec;
}

void KMaxDistance::copyVals( const MaxRec *src, MaxRec *dst ){
    if ( src->numVals != dst->numVals ){ fprintf(stderr, "ERROR: size mismatch in copyVals.\n"); exit(1); }
    for ( int i = 0; i < src->numVals; ++i ) dst->vals[i] = src->vals[i];
    dst->sumVals = src->sumVals;
}

int KMaxDistance::putValToItsSortedPlace( const double val, double *vals, const int numVals ){
    if ( val <= vals[numVals-1] ) return 0;
    for ( int i = 0; i < numVals; ++i ){
        if ( vals[i] >= val ) continue;
        for ( int j = numVals-1; j > i; --j ) vals[j] = vals[j-1];
        vals[i] = val;
        return 1;
    }
    return 0;
}

void KMaxDistance::modifyDstMaxs( MaxRec *dst, const double edgeCost ){
    double oldMinCost = dst->vals[dst->numVals-1];
    if ( putValToItsSortedPlace( edgeCost, &(dst->vals[0]), dst->numVals )) dst->sumVals += edgeCost - oldMinCost;
}

void KMaxDistance::analyseVecsOfMaxs( const double *vals1, const double *vals2, const int numVals, int *isBetterCnt1, int *isBetterCnt2 ){

    int better1[MAX_NUM_MAXS], better2[MAX_NUM_MAXS];

    int cntBetter1 = 0, cntBetter2 = 0;
    double sum1 = 0.0, sum2 = 0.0;
    for ( int numRetained = 1; numRetained <= numVals; ++numRetained ){
        sum1 += vals1[numRetained-1];
        sum2 += vals2[numRetained-1];
        if ( sum1 < sum2 - GEO_MAX_EPSX ){ better1[numRetained] = 1; cntBetter1++; } else better1[numRetained] = 0;
        if ( sum2 < sum1 - GEO_MAX_EPSX ){ better2[numRetained] = 1; cntBetter2++; } else better2[numRetained] = 0;
    }

    //printf( "better1: " ); for ( int i = 1; i <= numVals; ++i ) printf( "%d ", better1[i] ); printf( "\n" );
    //printf( "better2: " ); for ( int i = 1; i <= numVals; ++i ) printf( "%d ", better2[i] ); printf( "\n" );

    if ( probabThresh > 0.0 && cntBetter1 < (2*cntBetter2/4)){
        for ( int numRetained = 1; numRetained <= numVals; ++numRetained ){
            if ( better1[numRetained] ){
                int numReplaced = numVals-numRetained, pathLenStep = numReplaced/2; if ( pathLenStep == 0 ) pathLenStep++;
                double probab1 = probabEstimator->getMaxProbab( vals1, numReplaced, numReplaced, 3*numReplaced, pathLenStep );
                if ( probab1 < probabThresh ) better1[numRetained] = 0;
            }
        }
    }

    if ( probabThresh > 0.0 && cntBetter2 < (2*cntBetter1/4)){
        for ( int numRetained = 1; numRetained <= numVals; ++numRetained ){
            if ( better2[numRetained] ){
                int numReplaced = numVals-numRetained, pathLenStep = numReplaced/2; if ( pathLenStep == 0 ) pathLenStep++;
                double probab2 = probabEstimator->getMaxProbab( vals2, numReplaced, numReplaced, 3*numReplaced, pathLenStep  );
                if ( probab2 < probabThresh ) better2[numRetained] = 0;
            }
        }
    }

    //printf( "better1: " ); for ( int i = 1; i <= numVals; ++i ) printf( "%d ", better1[i] ); printf( "\n" );
    //printf( "better2: " ); for ( int i = 1; i <= numVals; ++i ) printf( "%d ", better2[i] ); printf( "\n" );

    cntBetter1 = 0; cntBetter2 = 0;
    for ( int i = 1; i <= numVals; ++i ) if ( better1[i] ) cntBetter1++;
    for ( int i = 1; i <= numVals; ++i ) if ( better2[i] ) cntBetter2++;
    *isBetterCnt1 = cntBetter1;
    *isBetterCnt2 = cntBetter2;

}

//______________________________________________________________________________
// The function compares two max vectors after they would be modified by new edges with a certain cost.
// The possible results: 
// 0 the distance is always equalled whatever cost comes
// 1 the first vector is always better whatever cost comes
// 2 the second vector is always better ...
// 3 both vector have chance to be better, depends on the cost of new edge

int KMaxDistance::compareTwoVecsOfMaxs( MaxRec *rec1, MaxRec *rec2 ){

    int isBetterCnt1 = 0, isBetterCnt2 = 0;
    analyseVecsOfMaxs( rec1->vals, rec2->vals, /*rec1->numVals*/ numDistMaxsGl, &isBetterCnt1, &isBetterCnt2 );

    //printRec( "comp1: ", rec1, numDistMaxsGl );
    //printRec( "comp2: ", rec2, numDistMaxsGl );
    //printf( "isBetterCnt1=%2d isBetterCnt2=%2d\n", isBetterCnt1, isBetterCnt2 );

    if ( isBetterCnt1 == 0 && isBetterCnt2 == 0 ) return 0;
    if ( isBetterCnt1  > 0 && isBetterCnt2 == 0 ) return 1;
    if ( isBetterCnt1 == 0 && isBetterCnt2  > 0 ) return 2;
    if ( isBetterCnt1  > 0 && isBetterCnt2  > 0 ) return 3;
    return 0;
}
//______________________________________________________________________________
// The function tests vector newRec with the list of vectors connected to the nodeRec
// If a vector from the list is better than newRect, it returns 1
// If a vector from the list is equalled to the newRect, it returns 0
// If newRec is always better then a vector from the list, it returns 2 and the worse vector from the list is added to recsToOverWrite
// If newRec can not be compared, it returns 3

int KMaxDistance::compareVecOfMaxsWithAllVecsInNode( MaxRec *newRec, const MaxRec *nodeRec, MaxRec **recsToOverWrite, int *numToOverwrite ){

    int result = 0;
    *numToOverwrite = 0;
    MaxRec *rec = (MaxRec*)nodeRec;
    while ( rec != NULL ){
        if ( rec->isUsed ){
            result = compareTwoVecsOfMaxs( rec, newRec );
            if ( result <= 1 ) return result;
            if ( result == 2 ){
                recsToOverWrite[*numToOverwrite] = rec;
                (*numToOverwrite)++;
                if ( *numToOverwrite >= MAX_NUM_TO_OVERWRITE ){ printf( "MAX_NUM_TO_OVERWRITE is too small\n" ); exit(1); } 
            }
        }
        rec = (MaxRec*)rec->next;
    }
    //if ( *numToOverwrite > 1 ) printf( "numToOverwrite=%d\n", *numToOverwrite );
    if ( *numToOverwrite > 0 ) return 2;
    return 3;
}


void KMaxDistance::addFirstPathToNodeAVL( MaxRec *srcRec, const int idxOfDstNode, const double edgeCost,
    const int numDistMaxs, MaxRec **maxs, NodeAVL **queue ){

    MaxRec *tmpRec = (MaxRec*)createNewMaxRec( idxOfDstNode, numDistMaxs );
    copyVals( srcRec, tmpRec );
    modifyDstMaxs( tmpRec, edgeCost );
    //tmpRec->back = srcRec->imgIdx;
    tmpRec->back = srcRec;
    tmpRec->qPtr = avlInsert( queue, tmpRec->sumVals, tmpRec );
    maxs[idxOfDstNode] = tmpRec;
}

int KMaxDistance::getNumMaxRecs( MaxRec *srcRec ){
    int cnt = 0;
    MaxRec *rec = srcRec;
    while ( rec != NULL ){
        if ( rec->isUsed ) cnt++;
        rec = (MaxRec*)rec->next;
    }
    return cnt;
}

void KMaxDistance::freeMaxRecs( MaxRec *rec ){
    while ( rec != NULL ){
        MaxRec *nextRec = (MaxRec*)rec->next;
        free( rec );
        rec = nextRec;
    }
}
//______________________________________________________________________________
// The node contains the list with at least one max vector.
// A new max vector is coming to this node. It has to be decided whether the new node should be stored or thrown away.
// It is decided by the function compareVecOfMaxsWithAllVecsInNode. The values can be from 0 to 3:
// compare == 0: dstRec contains the same vector as tmpRec (tmpRec can be thrown away)
// compare == 1: tmpRec is worse than a vector from dstRec (tmpRec can be thrown away)
// compare == 2: tmpRec is better than a vector from dstRec, recToOverWrite contains pointer to the old (and always worse) vector.
//               Then, it is overwritten and its position in the max vector queue has to be changed.
//               Although the tmpRec is worse then the best vector in dstRec (that gives the distance to this node), 
//               tmpRec can be useful in computing the distance in other nodes.
//               Therefore, it is neccessary to modify the vectors in the nodes that have already found minimal distance.
// compare == 3: It can not be decided which vector is better, both are stored
//               The new vector is added to the list and added to the max vector queue
// Note: better vector is the vector that gives lower distance after it is modified by a random number of edges with a certain cost

void KMaxDistance::addFurtherPathToNodeAVL( MaxRec *srcRec, const int idxOfDstPix, const double edgeCost,
    const int numDistMaxs, MaxRec **maxs, NodeAVL **queue ){

    MaxRec *dstRec = maxs[idxOfDstPix];

    MaxRec *tmpRec = (MaxRec*)createNewMaxRec( dstRec->imgIdx, numDistMaxs );

    //tmpRec->back = srcRec->imgIdx;
    tmpRec->back = srcRec;
    copyVals( srcRec, tmpRec );
    modifyDstMaxs( tmpRec, edgeCost );
    //printRec( "modif: ", tmpRec, numDistMaxsGl );

    int numRecsToOverwrite;
    MaxRec* recsToOverWrite[MAX_NUM_TO_OVERWRITE];
    int compare = compareVecOfMaxsWithAllVecsInNode( tmpRec, dstRec, recsToOverWrite, &numRecsToOverwrite );
    MaxRec* recToOverWrite;
    //printf( "compare=%d\n", compare );

    switch ( compare ){

      case 2: recToOverWrite = recsToOverWrite[0];
              //recToOverWrite->back = srcRec->imgIdx;
              recToOverWrite->back = srcRec;
              copyVals( tmpRec, recToOverWrite );
              if ( recToOverWrite->wasMin ) recToOverWrite->qPtr = avlInsert( queue, recToOverWrite->sumVals, recToOverWrite );
              else avlUpdateNode( queue, recToOverWrite->qPtr, recToOverWrite->sumVals );
              free( tmpRec ); numMaxRecs--;
              for ( int i = 1; i < numRecsToOverwrite; ++i ) 
              {
                  recsToOverWrite[i]->isUsed = 0;
                  //MaxRec* prec = (MaxRec*)recsToOverWrite[i]->prev;
                  //MaxRec* nrec = (MaxRec*)recsToOverWrite[i]->next;
                  //if(prec != NULL) prec->next = nrec;
                  //if(nrec != NULL) nrec->prev = prec;
                  //free(recsToOverWrite[i]);
              }
              numMaxRecs = numMaxRecs - numRecsToOverwrite + 1;
              break;

      case 3:
              tmpRec->next = maxs[dstRec->imgIdx];
              //maxs[dstRec->imgIdx]->prev = tmpRec;
              tmpRec->qPtr = avlInsert( queue, tmpRec->sumVals, tmpRec );
              maxs[dstRec->imgIdx] = tmpRec;
              break;

      default: free( tmpRec ); numMaxRecs--; break;

    }
    
    #ifdef DEBUG_MODE
    int cnt = getNumMaxRecs( maxs[idxOfDstPix] );
    if ( cnt > numMaxRecsInOneNode ) numMaxRecsInOneNode = cnt;
    #endif

    #ifdef CHECK_NODES
    MaxRec *iRec = maxs[idxOfDstPix];
    while ( iRec != NULL ){
        if ( iRec->isUsed ){
            MaxRec *jRec = (MaxRec*)iRec->next;
            while ( jRec != NULL ){
               if ( jRec->isUsed ){
                   int result = compareTwoVecsOfMaxs( iRec, jRec );
                   if ( result != 3 ){
                       printf( "IS BAD: idxOfDstPix=%6d result=%d cnt=%2d  --->  ", idxOfDstPix, result, cnt );
                       printVals( iRec->vals, iRec->numVals, numDistMaxs );
                       printVals( jRec->vals, jRec->numVals, numDistMaxs );
                       printf( "\n" );
                   }
               }
               jRec = (MaxRec*)jRec->next;
            }
        }
        iRec = (MaxRec*)iRec->next;
    }
    #endif

}
//______________________________________________________________________________

void KMaxDistance::addPathToNodeAVL( MaxRec *srcRec, const int dstNodeIdx, const double edgeCost,
    const int numDistMaxs, MaxRec **maxs, NodeAVL **queue ){
    
    if ( maxs[dstNodeIdx] == NULL ) addFirstPathToNodeAVL( srcRec, dstNodeIdx, edgeCost, numDistMaxs, maxs, queue );
    else addFurtherPathToNodeAVL( srcRec, dstNodeIdx, edgeCost, numDistMaxs, maxs, queue );

}
//______________________________________________________________________________

double KMaxDistance::compute( const double *xEdgeCosts, const double *yEdgeCosts, const int width, const int height, const int numDistMaxs,
    const int *srcPixs, const int numSrcPixs, const int dstPix, double *dists, int* backPtrs, double *maxVector ){

    if ( numDistMaxs > MAX_NUM_MAXS ){ fprintf(stderr, "ERROR: numDistMaxs is too big (increase MAX_NUM_MAXS).\n" ); return 0.0; }
    if ( numDistMaxs < 1 ){ fprintf(stderr, "ERROR: numDistMaxs has to be positive.\n" ); return 0.0; }
    if ( numSrcPixs < 1 ){ fprintf(stderr, "ERROR: no source points found.\n" ); return 0.0; }

    numMaxRecs = 0;
    numMaxRecsInOneNode = 0;
    numDistMaxsGl = numDistMaxs;

	int cnt = 0;
    //double *dists;
    if ( dists == NULL ) dists = (double*)malloc( width*height*sizeof( dists[0] )); // else dists = distsExt; 
    MaxRec **maxs = (MaxRec**)malloc( width*height*sizeof( maxs[0] ));
    MaxRec* prevPtrs = (MaxRec*)malloc( width*height*sizeof( prevPtrs[0] ));
    
    for ( int i = 0; i < width*height; ++i ) maxs[i] = NULL;
    for ( int i = 0; i < width*height; ++i ) dists[i] = MAX_GEO_DIST;
    //if ( backPtrs != NULL ) for ( int i = 0; i < width*height; ++i ) backPtrs[i] = -1;

    NodeAVL *queue = NULL;

    for ( int i = 0; i < numSrcPixs; ++i ){
        int imgIdx = srcPixs[i];
        MaxRec *rec = createNewMaxRec( imgIdx, numDistMaxs );
        rec->wasMin = 1;
        rec->qPtr = avlInsert( &queue, 0.0, rec );
        maxs[imgIdx] = rec;
        if ( prevPtrs != NULL ) prevPtrs[imgIdx] = *rec;
    }

	
    while ( queue != NULL )
    {

        MaxRec *minDistRec = (MaxRec*)avlFindAndDeleteMin( &queue );

        if ( !minDistRec->isUsed ) continue;
        
        minDistRec->wasMin = 1;
        int imgIdx = minDistRec->imgIdx;
        
        if ( minDistRec->sumVals < dists[imgIdx] ){ 
            if ( dists[imgIdx] > MAX_GEO_DIST - GEO_MAX_EPSX ) cnt++;
            dists[imgIdx] = minDistRec->sumVals;
            prevPtrs[imgIdx] = *minDistRec;
            if ( cnt == width*height ) break;
        }
        
        if ( dstPix >= 0 ){ if ( dists[dstPix] < MAX_GEO_DIST ) break; }
        
        int y = imgIdx/width, x = imgIdx - y*width;
        if ( x > 0 )        addPathToNodeAVL( minDistRec,     imgIdx-1,     xEdgeCosts[imgIdx-1], numDistMaxs, maxs, &queue );
        if ( x < width-1 )  addPathToNodeAVL( minDistRec,     imgIdx+1,       xEdgeCosts[imgIdx], numDistMaxs, maxs, &queue );
        if ( y > 0 )        addPathToNodeAVL( minDistRec, imgIdx-width, yEdgeCosts[imgIdx-width], numDistMaxs, maxs, &queue );
        if ( y < height-1 ) addPathToNodeAVL( minDistRec, imgIdx+width,       yEdgeCosts[imgIdx], numDistMaxs, maxs, &queue );

    }


    double retVal = 0.0;
    retVal = dists[abs(dstPix)];

    if ( maxVector != NULL ) 
    {
        MaxRec* m = maxs[abs(dstPix)];
        double minSum = m->sumVals;
        for(int i = 0; i < m->numVals; i++) maxVector[i] = m->vals[i];

        while(m->next != NULL)
        {
            m = (MaxRec*) m->next;
            if(m->sumVals < minSum)
            {
                minSum = m->sumVals;
                for(int i = 0; i < m->numVals; i++) maxVector[i] = m->vals[i];
            }
        }
    }

    if ( backPtrs != NULL ) 
    {
        int backIdx = 0;
        MaxRec* backRec = &prevPtrs[abs(dstPix)];
        while(backRec != NULL)
        {
            backPtrs[backIdx++] = backRec->imgIdx;
            backRec = (MaxRec*) backRec->back;
        }
        backPtrs[backIdx++] = -1;
    }
    
    while ( queue != NULL ) avlFindAndDeleteMin( &queue );
    
    #ifdef DEBUG_MODE
    meanNumRecs = (double)numMaxRecs/(double)(width*height);
    maxNumRecsInNode = numMaxRecsInOneNode;
    printf("meanNumRecs: %.2lf\n", meanNumRecs);
    printf("maxNumRecsInNode: %d\n", maxNumRecsInNode);
    #endif

    for ( int i = 0; i < width*height; ++i ) freeMaxRecs( maxs[i] );
    free( maxs ); 
    free( queue );
    free( prevPtrs );
    //if ( distsExt == NULL ) free( dists );

    return retVal;

}

double KMaxDistance::compute( const Image<unsigned char>& inputImage, const int numDistMaxs, const int* srcPixs, const int numSrcPixs, const int dstPix,
    Image<double> *distImg, std::vector<int>* backPtrs, std::vector<double>* maxVector, const double sigmaForWeight, const double baseCost)
{
    if(numSrcPixs == 0) { fprintf(stderr, "ERROR: source point is not found\n"); return -1.0; }
    
    int imgWidth = inputImage.width;
    int imgHeight = inputImage.height;
    int numPixs = inputImage.size;
    
    double *image = new double[numPixs];
    double *xEdgeCosts = new double[numPixs];
    double *yEdgeCosts = new double[numPixs];
    double *dists = NULL; 
    if(distImg != NULL) dists = new double[numPixs];
    int *backTracePoints = NULL;
    if(backPtrs != NULL) backTracePoints = new int[numPixs];
    double *vecOfMaxs = NULL;
    if(maxVector != NULL) vecOfMaxs = new double[numDistMaxs];
    
    for(int i = 0; i < numPixs; i++) image[i] = (double)inputImage.vals[i]/255.0;

    edgeCosts(image, imgWidth, imgHeight, sigmaForWeight, baseCost, xEdgeCosts, yEdgeCosts);
    probabEstimator = new ProbabEstimator( "distrib4", numDistMaxs );
    
    double dist = compute( xEdgeCosts, yEdgeCosts, imgWidth, imgHeight, numDistMaxs, srcPixs, numSrcPixs, dstPix, dists, backTracePoints, vecOfMaxs );
    
    if(backPtrs != NULL)
    {
        int i = 0;
        while(backTracePoints[i] > -1) { backPtrs->push_back(backTracePoints[i]); i++; }
    }
    
    if(maxVector != NULL)
    {
        for(int i = 0; i < numDistMaxs; i++) maxVector->push_back(vecOfMaxs[i]);
    }
    
    if(distImg != NULL) memcpy(distImg->vals, dists, numPixs * sizeof(dists[0]) );

    delete[] image ; delete[] xEdgeCosts ; delete[] yEdgeCosts ; 
    if(dists != NULL) delete[] dists; 
    if(backTracePoints != NULL) delete[] backTracePoints; 
    if(vecOfMaxs != NULL) delete[] vecOfMaxs;
    delete(probabEstimator);

    return dist;
}

void KMaxDistance::edgeCosts( const double *image, const int width, const int height,
    const double brSigma, const double sxy, double *xCosts, double *yCosts ){
    
    //double sxy2 = sxy*sxy;
    for ( int y = 0; y < height; ++y ){
        for ( int x = 0; x < width-1; ++x ){
            int idx = y*width + x;
            //double dbr = gaussInv( image[idx] - image[idx+1], brSigma );
            //xCosts[idx] = sqrt( sxy2 + dbr*dbr );
            xCosts[idx] = fabs(image[idx] - image[idx+1]) + sxy;
        }
    }
    for ( int y = 0; y < height-1; ++y ){
        for ( int x = 0; x < width; ++x ){
            int idx = y*width + x;
            //double dbr = gaussInv( image[idx] - image[idx+width], brSigma );
            //yCosts[idx] = sqrt( sxy2 + dbr*dbr );
            yCosts[idx] = fabs(image[idx] - image[idx+width]) + sxy;
        }
    }
}


//______________________________________________________________________________

ProbabEstimator::ProbabEstimator( const char *fileName, const int numMaxs ){
    this->numMaxs = numMaxs;
    distribFunction = new DistribFunction( fileName );

}

ProbabEstimator::~ProbabEstimator( void ){
    delete distribFunction;
}
//______________________________________________________________________________
// Probability than more than q numbers will be greater than a_(r), alias minValInRetained.
// It can not happen to change q incoming numbers, more than q numbers will be replaced.
// The value pow( cPAr1, (double)i )*pow(PAr1, (double)(pathLength-i)) is the probability that i numbers is
// greater, the rest of numbers are lower. The number of combinations is given by combNumber.
// The two functions follow, the second version may have less numbers to add if pathLength is high.
// Among these two functions, there are two fast versions - one with an array; second without.
// The array version may be suitable for a possible speeding up, when it is computed for various values of pathLength,
// which has not been done yet. The version without array is faster.

double ProbabEstimator::getProbabGt0( const double minValInRetained, const int numReplaced, const int pathLength ){

    double  PAr = distribFunction->getDistrib(minValInRetained);
    double cPAr = 1.0-PAr;

    double val = 0.0;
    for ( int i = numReplaced+1; i <= pathLength; ++i ){
        double val0 = (double)combNumber( pathLength, i )*pown( cPAr, i )*pown(PAr, (pathLength-i));
        val += val0;
    }

    return val;
}

double ProbabEstimator::getProbabGt( const double minValInRetained, const int numReplaced, const int pathLength ){

    double  PAr = distribFunction->getDistrib(minValInRetained);
    double cPAr = 1.0-PAr;

    double val = 1.0;
    for ( int i = 0; i <= numReplaced; ++i ){
        val -= (double)combNumber( pathLength, i )*pown( cPAr, i )*pown(PAr, (pathLength-i));
    }

    return val;
}

double ProbabEstimator::getProbabGtFast1( const double minValInRetained, const int numReplaced, const int pathLength ){

    double  PAr = distribFunction->getDistrib( minValInRetained );
    double cPAr = 1.0-PAr;
    double vals[20]; vals[0] = 1.0;

    double val = 1.0;
    for ( int i = 0; i <= numReplaced; ++i ){ vals[i] = val; val *= (double)(pathLength-i)/(double)(i+1); }
    val = cPAr; 
    for ( int i = 1; i <= numReplaced; ++i ){ vals[i] *= val; val *= cPAr; }
    val = pown( PAr, pathLength-numReplaced );
    for ( int i = numReplaced; i >= 0; --i ){ vals[i] *= val; val *= PAr; }
    val = 1.0;
    for ( int i = 0; i <= numReplaced; ++i ) val -= vals[i];
    return val;
}

double ProbabEstimator::getProbabGtFast2( const double minValInRetained, const int numReplaced, const int pathLength ){
    double  PAr = distribFunction->getDistrib( minValInRetained );
    double cPAr = 1.0-PAr, invPAr = 1.0/PAr;
    double val = pown( PAr, pathLength ), sum = 1.0;
    for ( int i = 0; i <= numReplaced; ++i ){ sum -= val; val *= cPAr*invPAr*(double)(pathLength-i)/(double)(i+1); }
    return sum;
}

//______________________________________________________________________________
// Probability that less than q numbers will be greater than a_(r+1), alias maxValInReplaced.
// It can not happen to change q incoming numbers, because lower amount of values comes. Therefore, less than q numbers will be replaced.
// The value pow( cPAr1, (double)i )*pow(PAr1, (double)(pathLength-i)) is the probability that i numbers are
// lower, the rest of numbers are greater. The number of combinations is given by combNumber.
// The three versions of the function exist, basic, fast with array, and fast without array.
// The array version may be suitable for a possible speeding up, when it is computed for various values of pathLength,
// which has not been done yet.

double ProbabEstimator::getProbabLt( const double maxValInReplaced, const int numReplaced, const int pathLength ){

    double  PAr1 = distribFunction->getDistrib( maxValInReplaced );
    double cPAr1 = 1.0-PAr1;

    double val = 0.0;
    for ( int i = 0; i < numReplaced; ++i ){
        val += (double)combNumber( pathLength, i )*pown( cPAr1, i )*pown(PAr1, (pathLength-i));
    }
    return val;
}

double ProbabEstimator::getProbabLtFast1( const double maxValInReplaced, const int numReplaced, const int pathLength ){
    double  PAr1 = distribFunction->getDistrib( maxValInReplaced );
    double cPAr1 = 1.0-PAr1;
    double vals[20]; vals[0] = 1.0;

    double val = 1.0;
    for ( int i = 0; i < numReplaced; ++i ){ vals[i] = val; val *= (double)(pathLength-i)/(double)(i+1); }
    val = cPAr1; 
    for ( int i = 1; i < numReplaced; ++i ){ vals[i] *= val; val *= cPAr1; }
    val = pown( PAr1, pathLength-numReplaced+1 );
    for ( int i = numReplaced-1; i >= 0; --i ){ vals[i] *= val; val *= PAr1; }
    val = 0.0;
    for ( int i = 0; i < numReplaced; ++i ) val += vals[i];
    return val;
}

double ProbabEstimator::getProbabLtFast2( const double maxValInReplaced, const int numReplaced, const int pathLength ){
    double  PAr1 = distribFunction->getDistrib( maxValInReplaced );
    double cPAr1 = 1.0-PAr1, invPAr1 = 1.0/PAr1;
    double val = pown( PAr1, pathLength ), sum = 0.0;
    for ( int i = 0; i < numReplaced; ++i ){ sum += val; val *= cPAr1*invPAr1*(double)(pathLength-i)/(double)(i+1); }
    return sum;
}
//______________________________________________________________________________

double ProbabEstimator::getProbabFast( const double *vecMaxs, const int numReplaced, const int pathLength ){
    if ( numReplaced == 0 ) return 1.0 - getProbabGtFast2( vecMaxs[numMaxs-1], 0, pathLength );
    if ( numReplaced == numMaxs ) return 1.0 - getProbabLtFast2( vecMaxs[0], numMaxs, pathLength );
    return 1.0 - getProbabLtFast2( vecMaxs[numMaxs-numReplaced], numReplaced, pathLength ) 
               - getProbabGtFast2( vecMaxs[numMaxs-numReplaced-1], numReplaced, pathLength );
}

double ProbabEstimator::getProbab( const double *vecMaxs, const int numReplaced, const int pathLength ){
    if ( numReplaced == 0 ) return 1.0 - getProbabGt( vecMaxs[numMaxs-1], 0, pathLength );
    if ( numReplaced == numMaxs ) return 1.0 - getProbabLt( vecMaxs[0], numMaxs, pathLength );
    return 1.0 - getProbabLt( vecMaxs[numMaxs-numReplaced], numReplaced, pathLength ) 
               - getProbabGt( vecMaxs[numMaxs-numReplaced-1], numReplaced, pathLength );
}
//______________________________________________________________________________

double ProbabEstimator::getMaxProbab( const double *vecMaxs, const int numReplaced, const int minPathLength, const int maxPathLength, const int pathLengthStep ){
    double maxProbab = 0.0;
    for ( int pathLength = minPathLength; pathLength <= maxPathLength; pathLength += pathLengthStep ){
        double probabF = getProbabFast( vecMaxs, numReplaced, pathLength );
        #ifdef CHECK_FAST_PROBABS
        double probabS = getProbab( vecMaxs, numReplaced, pathLength );
        if ( fabs(probabF-probabS) > 1.0e-6 ) printf( "WARNING: Probabilities mismatch (%.6f %.6f).\n", probabF, probabS );
        #endif
        if ( probabF > maxProbab ) maxProbab = probabF;
    }
    return maxProbab;
}

double ProbabEstimator::combNumber( const int n, const int k ){
    double val = 1.0;
    for ( double nd = (double)n; nd > (double)(n-k) + 0.5; nd -= 1.0 ) val *= nd;
    for ( double kd = (double)k; kd > 1.0; kd -= 1.0 ) val /= kd;
    return val;
}

double ProbabEstimator::pown(double base, int exp)
{
    double result = 1.0;
    while (exp)
    {
        if (exp & 1)
            result *= base;
        exp >>= 1;
        base *= base;
    }

    return result;
}
//______________________________________________________________________________

