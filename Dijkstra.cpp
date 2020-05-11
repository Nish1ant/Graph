#include "Dijkstra.h"

//
// Check if a vertex has been visited
// i.e, perform linear search on the visitedPath vector
//
bool checkVisited(long long vertex, set<long long> &visited)
{
    if (visited.count(vertex) > 0) {
        return true;
    }

    // If get here, vertex not found:
    return false;
}

//
// For graph G, find the minimum distances to all other vertex using Dijkstra's algorithm
// Return these distances in a map (distances)
// Also build the spanning tree for the graph using the unordered_map (predecessors)
//
void Dijkstra(graph<long long , double>& G, 
			  long long startV, unordered_map<long long, long long> &predecessors,
              map<long long, double>& distances)
{
	set<long long> visited;
	vector<long long> vertices = G.getVertices();
	priority_queue<pair<long long, double>, vector<pair<long long, double>>, prioritize> unvisitedQueue;
	set<long long> neighbors;

    //
	// Add all vertices to unvisitedQueue
	// Start with distances being INF
    //
	for (auto& vertex : vertices) {
		distances[vertex] = INF;
		unvisitedQueue.push(make_pair(vertex, INF));
	}

    //
	// Push start vertex to unvisitedQueue. 
    // Distance from the note to itself is 0.0 miles
    //
    predecessors[startV] = -1;
	distances[startV] = 0.0;
	unvisitedQueue.push(make_pair(startV, 0.0));


    long long currentV;
    double currentDist, edgeWeight, altDist;
    while (!unvisitedQueue.empty())
    {
        // Pop the top of the queue:
        currentV = (unvisitedQueue.top()).first;
        currentDist = (unvisitedQueue.top()).second;
        unvisitedQueue.pop();

        // Stop the loop when you hit infinity - there will be no shorter distances    
        if (currentDist == INF)
            break;

        // Skip over current iteration if the vertex has already been visited
        else if (checkVisited(currentV, visited))
            continue;

        // "Visit" current vertex
        else
            visited.insert(currentV);

        neighbors = G.neighbors(currentV);
        for (auto& neighbor : neighbors) {
            // Get total distance from startV to currentV's neighbor
            G.getWeight(currentV, neighbor, edgeWeight);
            altDist = currentDist + edgeWeight;

            // Update distances if shorter path was found:
            if (altDist < distances[neighbor]) {
                predecessors[neighbor] = currentV;
                distances[neighbor] = altDist;
                unvisitedQueue.push(make_pair(neighbor, altDist));
            }
        }
    }

}
