/*graph.h*/

//
// Nishant Chudasama
// original author: Prof. Joe Hummel
//
// Graph class using adjacency list representation. 
// Implemented using nested maps 
//
// Project 7 Part 1
// U. of Illinois, Chicago
// CS 251: Spring 2020
//

#pragma once

#include <iostream>
#include <stdexcept>
#include <vector>
#include <set>
#include <map>

using namespace std;

template<typename VertexT, typename WeightT>
class graph
{
private:

  typedef map<VertexT, WeightT> neighbor;
  map<VertexT, neighbor> adjList;
  int numVertices;
  int numEdges;

public:
  //
  // constructor:
  //
  graph()
  {
      // Default constuctor for std::map invoked
      numVertices = 0;
      numEdges = 0;
  }

  //
  // NumVertices
  //
  // Returns the # of vertices currently in the graph.
  //
  int NumVertices() const
  {
      return numVertices;
  }

  //
  // NumEdges
  //
  // Returns the # of edges currently in the graph.
  //
  int NumEdges() const
  {
    return numEdges;
  }

  //
  // addVertex
  //
  // Adds the vertex v to the graph if there's room, and if so
  // returns true.  If the graph is full, or the vertex already
  // exists in the graph, then false is returned.
  //
  bool addVertex(VertexT v)
  {

    //
    // is the vertex already in the graph?  If so, we do not 
    // insert again otherwise Vertices may fill with duplicates:
    //
    if (adjList.count(v) > 0)
      return false;

    //
    // if we get here, vertex does not exist so insert.  
    //
    neighbor M;                       // Empty map for the neighbors
    adjList.insert(make_pair(v, M));
    ++numVertices;                    // Increase the number of vertices
     
    return true;
  }

  //
  // addEdge
  //
  // Adds the edge (from, to, weight) to the graph, and returns
  // true.  If the vertices do not exist or for some reason the
  // graph is full, false is returned.
  //
  // NOTE: if the edge already exists, the existing edge weight
  // is overwritten with the new edge weight.
  //
  bool addEdge(VertexT from, VertexT to, WeightT weight)
  {
    // Check if both vertices exist in the graph:
    if (adjList.count(from) == 0 || adjList.count(to) == 0)
        return false;

    auto itr = adjList.find(from);
    neighbor& M = itr->second;

    // Increase the number of edges if we added a new edge:
    if (M.count(to) == 0)
        ++numEdges;

    M[to] = weight;

    return true;
  }

  //
  // getWeight
  //
  // Returns the weight associated with a given edge.  If 
  // the edge exists, the weight is returned via the reference
  // parameter and true is returned.  If the edge does not 
  // exist, the weight parameter is unchanged and false is
  // returned.
  //
  bool getWeight(VertexT from, VertexT to, WeightT& weight) const
  {
    // Check if both vertices exist:
    auto itr = adjList.find(from);
    if (itr == adjList.end() || adjList.count(to) == 0)
        return false;

    //
    // the vertices exist, but does the edge exist?
    //
    neighbor M = itr->second;
    auto it2 = M.find(to);
    if (it2 == M.end())  // no:
      return false;

    //
    // Okay, the edge exists, return the weight via the 
    // reference parameter:
    //
    weight = it2->second;

    return true;
  }

  //
  // neighbors
  //
  // Returns a set containing the neighbors of v, i.e. all
  // vertices that can be reached from v along one edge.
  // Since a set is returned, the neighbors are returned in
  // sorted order; use foreach to iterate through the set.
  //
  set<VertexT> neighbors(VertexT v) const
  {
    set<VertexT>  S;

    // 
    // Check if the vertex exists:
    //
    auto itr = adjList.find(v);
    if (itr == adjList.end()) {
        return S;                 // Return empty set, vertex not found
    }

    //
    // We found the vertex exists, so loop through its neighbors 
    // and add each neighbor to the set:
    //   
    neighbor M = itr->second;
    for (auto vertex : M) {
        S.insert(vertex.first);
    }

    return S;
  }

  //
  // getVertices
  //
  // Returns a vector containing all the vertices currently in
  // the graph.
  //
  vector<VertexT> getVertices() const
  {
      vector<VertexT> vertices;

      for (auto vertex : adjList) {
          vertices.push_back(vertex.first);
      }

      return vertices;
  }

  //
  // dump
  // 
  // Dumps the internal state of the graph for debugging purposes.
  //
  // Example:
  //    graph<string,int>  G;
  //    ...
  //    G.dump(cout);  // dump to console
  //
  void dump(ostream& output) const
  {
    output << "***************************************************" << endl;
    output << "********************* GRAPH ***********************" << endl;

    output << "**Num vertices: " << this->NumVertices() << endl;
    output << "**Num edges: " << this->NumEdges() << endl;

    output << endl;
    output << "**Vertices:" << endl;
    int numVertex = 0;
    for (auto vertex: adjList)
    {
        output << " " << numVertex << ". " << vertex.first;
        cout << endl;
        ++numVertex;
    }

    output << endl;
    output << "**Edges:" << endl;
    neighbor M;
    VertexT currentVertex;
    for (auto itr = adjList.begin(); itr != adjList.end(); ++itr)
    {
        currentVertex = itr->first;
        output << currentVertex << ":";
        M = itr->second;

        for (auto vertex : M) {
            output << " (" << currentVertex << ","
                   << vertex.first << "," << vertex.second << ")";
        }

        cout << endl;
    }

    output << "**************************************************" << endl;
  }

};