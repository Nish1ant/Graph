/* Dijkstra.h */

//
// Contains definitions for functions required to implement Dijksta's Algorithm
// to find the shortest weighted path
//

#pragma once

#include <vector>
#include <map>
#include <unordered_map>
#include <limits>
#include <queue>

#include "graph.h"
#include "osm.h"

using namespace std;

const double INF = numeric_limits<double>::max();

bool checkVisited(long long vertex, vector<long long>& visited);   

void Dijkstra(graph<long long, double>& G, long long startV,
			  unordered_map<long long, long long>& predecessors, 
			  map<long long, double>& distances);

// The priority queue is a min heap
// First order by the distance
// When distances are same, order by ID
class prioritize
{
public:
	bool operator() (const pair<long long, double>& p1, const pair<long long, double>& p2)
	{
		if (p1.second > p2.second)
			return true;

		else if (p1.second == p2.second) {
			return p1.first > p2.first;
		}

		else
			return false;
	}
};