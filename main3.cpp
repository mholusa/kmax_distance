/*============================================================================*/
/* main3.cpp                                                                  */
/*                                                        mrl vsb-tuo 2014-18 */
/*                                                                            */
/* the distance is computed in the following graph:                           */
/*                                                                            */
/*         0.3                                                                */
/*      1-------2                                                             */
/* 0.4 /         \ 0.2                                                        */
/*    /           \                                                           */
/*   O------3------4------5                                                   */
/*     0.5    0.1    0.3                                                      */
/*                                                                            */
/* the graph nodes are indexed from 0                                         */
/*============================================================================*/

#include "kmax_distance.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

/*============================================================================*/

int main( int argc, char** argv )
{
    Graph g;
	g.addNode();
	g.addNode();
	g.addNode();
	g.addNode();
	g.addNode();
	g.addNode();
	g.addEdge(0, 1, 0.4);
	g.addEdge(1, 2, 0.3);
	g.addEdge(2, 4, 0.2);
	g.addEdge(0, 3, 0.5);
	g.addEdge(3, 4, 0.1);
	g.addEdge(4, 5, 0.3);
	g.printInfo();
	 
    KMaxDistance kmax;
    
    const int numSeedPixs = 1;
    int seedPixs[numSeedPixs];
    seedPixs[0] = 0;
    int dstPix = 5;
    
    std::vector<int> backPoints;
    std::vector<double> maxVector;
    std::vector<double> distsGraph;
    int k = 2;
    
    double dist = kmax.compute(g, k, seedPixs, numSeedPixs, -dstPix, &distsGraph, &backPoints, &maxVector);
    printf("Distance: %.3f\n", dist);
    
    printf("Max vector: ( "); 
    for(int i = 0; i < k; i++) printf("%.3f ", maxVector[i]);
    printf(")\n");
    
    printf("Distances to nodes:\n");
	for (unsigned int i = 0; i < distsGraph.size(); i++)
	{
		printf("Distance to node %d: %.3f\n", i, distsGraph[i]);
	}
   
    printf("Backtracking:\n");
    int i = 0;
    while(backPoints[i] != -1)
    {
		printf("%d ", backPoints[i++]);
	}
	printf("\n");
    
    return 0;
}
/*============================================================================*/

