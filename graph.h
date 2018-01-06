#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <stdio.h>
#include <vector>

typedef struct{ int targetId; double cost;} Edge;

class Graph
{
	public:
		int size; 
		std::vector<std::vector<Edge>> nodes;
    
    Graph(int size) { 
        this->size = size; 
        this->nodes.reserve(size);
    }
    
    Graph() { 
        this->size = 0; 
    }
    
    void addNode() 
    {
		std::vector<Edge> edges;
		this->nodes.push_back(edges);
		this->size++;
	}
	
	void addEdge(int node1Id, int node2Id, double cost) 
    {
		if(node1Id == node2Id) return;
		if(node1Id >= this->size || node2Id >= this->size) { printf("Nodes not found\n"); return; }
		
		Edge e1 = {node2Id, cost};
		Edge e2 = {node1Id, cost};
		this->nodes[node1Id].push_back(e1);
		this->nodes[node2Id].push_back(e2);
	}
    
    void printInfo()
    {
		for(unsigned int i = 0; i < this->nodes.size(); i++)
		{
			printf("Node %d\n", i);
			for(unsigned int j = 0; j < this->nodes[i].size(); j++)
			{
				printf("Edge to node %d with cost %f\n", this->nodes[i][j].targetId, this->nodes[i][j].cost);
			}  
		}
      
	}
    
    ~Graph() {}
};


#endif
