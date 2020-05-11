/*main.cpp*/

//
// Nishant Chudasama
// U. of Illinois, Chicago
// CS 251: Spring 2020
// Project #07 Part 2: open street maps, graphs, and Dijkstra's alg
//
// Find shortest path between 2 buildings on the UIC East Campus
// (using Dijkstra's algorithm)
//
// There must exist a direct path between the 2 buildings,
// consisting of footpaths (program does not consider streets)
//
// References:
// TinyXML: https://github.com/leethomason/tinyxml2
// OpenStreetMap: https://www.openstreetmap.org
// OpenStreetMap docs:  
//   https://wiki.openstreetmap.org/wiki/Main_Page
//   https://wiki.openstreetmap.org/wiki/Map_Features
//   https://wiki.openstreetmap.org/wiki/Node
//   https://wiki.openstreetmap.org/wiki/Way
//   https://wiki.openstreetmap.org/wiki/Relation
//

#include <iostream>
#include <iomanip>  /*setprecision*/
#include <string>
#include <vector>
#include <map>
#include <stack>
#include <cstdlib>
#include <cstring>
#include <cassert>

#include "tinyxml2.h"
#include "dist.h"
#include "osm.h"
#include "graph.h"
#include "Dijkstra.h"

using namespace std;
using namespace tinyxml2;

//
// Function to add edges to the graph:
//
void addEdge(graph<long long, double>&G, 
             map<long long, Coordinates> &Nodes, 
             vector<FootwayInfo> &Footways)
{
    //
    // Loop through footways and add edges between all nodes:
    //
    double lat1, long1, lat2, long2, dist;
    map<long long, Coordinates>::iterator it;
    for (auto& footWay : Footways) {
        for (size_t i = 0; i < footWay.Nodes.size() - 1; ++i) {

            // Get coordinates for N1:
            it = Nodes.find(footWay.Nodes[i]);
            lat1 = it->second.Lat;
            long1 = it->second.Lon;

            // Get coordinates for N2:
            it = Nodes.find(footWay.Nodes[i + 1]);
            lat2 = it->second.Lat;
            long2 = it->second.Lon;

            dist = distBetween2Points(lat1, long1, lat2, long2);

            // Add edge in both directions (from N1 - N2 and from N2 - N1)
            // The weight is the distance between 2 nodes
            G.addEdge(footWay.Nodes[i], footWay.Nodes[i + 1], dist);
            G.addEdge(footWay.Nodes[i + 1], footWay.Nodes[i], dist);
        }
    }
}

//
// Function to find a building given the user input
//
bool findBuilding(string buildingName, vector<BuildingInfo>& Buildings, BuildingInfo &node)
{
    // lookup building by abbrev:
    for (auto& building : Buildings) {
        if (building.Abbrev == buildingName) {
            node = building;
            return true;
        }
    }

    // lookup building by full name (partial search):
    for (auto& building : Buildings) {
        if (building.Fullname.find(buildingName) != string::npos) {
            node = building;
            return true;
        }
    }

    // If get here, building not found:
    return false;
}

//
// Function to find the closest nodes (on a footway)
// to the start and destination buildings:
//
void findNearestNodes(long long &startId, long long &destId,
                      double &startLat, double &startLong,
                      double &destLat, double &destLong,
                      vector<FootwayInfo> &Footways, map<long long, Coordinates> &Nodes)
{
    double minStart = 99999999999.99;
    double minDest = 99999999999.99;
    double latitude, longitude, dist;
    Coordinates C;
    map<long long, Coordinates>::iterator it;

    //
    // Loop through all footways and find the nodes with minimum distance 
    // from the start and destination building
    //
    for (auto& footWay : Footways)
    {
        for (size_t i = 0; i < footWay.Nodes.size(); ++i) {
            it = Nodes.find(footWay.Nodes[i]);
            latitude = it->second.Lat;
            longitude = it->second.Lon;

            dist = distBetween2Points(startLat, startLong, latitude, longitude);

            if (dist < minStart) {
                minStart = dist;
                startId = it->first;
            }

            dist = distBetween2Points(destLat, destLong, latitude, longitude);

            if (dist < minDest) {
                minDest = dist;
                destId = it->first;
            }
        }
        
    }

    // Return the start and destination latitude and longitude by reference:
    it = Nodes.find(startId);
    startLat = it->second.Lat;
    startLong = it->second.Lon;

    it = Nodes.find(destId);
    destLat = it->second.Lat;
    destLong = it->second.Lon;

}

//
// Function to display the shortest path between the start and destination node
//
void displayShortestPath(map<long long, double>& distances,
                         unordered_map<long long, long long> &predecessors,
                         long long startId, long long destId)
{
    double totalDist = 0.0;
    stack<long long> path;

    long long currentV = destId;
    totalDist = distances[destId];

    if (totalDist == INF) {
        cout << "Sorry, destination unreachable" << endl;
        return;
    }

    cout << "Distance to dest: " << totalDist << " miles" << endl;
    cout << "Path: ";

    // Build stack of nodes along the path, working in reverse from destination
    // Use predecessors to find path to start
    while (currentV != -1)
    {
        path.push(currentV);
        currentV = predecessors[currentV];
    }

    // Display the entire path:
    cout << path.top();
    path.pop();
    while (!path.empty())
    {
        cout << "->" << path.top();
        path.pop();
    }
    cout << endl;
  
}

//////////////////////////////////////////////////////////////////
//
// main
//
int main()
{
    map<long long, Coordinates>  Nodes;     // maps a Node ID to it's coordinates (lat, lon)
    vector<FootwayInfo>          Footways;  // info about each footway, in no particular order
    vector<BuildingInfo>         Buildings; // info about each building, in no particular order
    XMLDocument                  xmldoc;
    graph<long long, double>     G;         // Vertices are nodes, weights are distances
    map<long long, double> distances;       // Map to store distances of all vertices from the start

    // Spanning tree for path implemented by storing predecessors of all vertices (-1 for start)
    unordered_map<long long, long long> predecessors;  


    cout << "** Navigating UIC open street map **" << endl;
    cout << endl;
    cout << std::setprecision(8);

    string def_filename = "map.osm";
    string filename;

    cout << "Enter map filename> ";
    getline(cin, filename);

    if (filename == "")
    {
        filename = def_filename;
    }

    //
    // Load XML-based map file 
    //
    if (!LoadOpenStreetMap(filename, xmldoc))
    {
        cout << "**Error: unable to load open street map." << endl;
        cout << endl;
        return 0;
    }

    //
    // Read the nodes, which are the various known positions on the map:
    //
    int nodeCount = ReadMapNodes(xmldoc, Nodes);

    //
    // Read the footways, which are the walking paths:
    //
    int footwayCount = ReadFootways(xmldoc, Footways);

    //
    // Read the university buildings:
    //
    int buildingCount = ReadUniversityBuildings(xmldoc, Nodes, Buildings);

    //
    // Stats
    //
    assert(nodeCount == Nodes.size());
    assert(footwayCount == Footways.size());
    assert(buildingCount == Buildings.size());

    cout << endl;
    cout << "# of nodes: " << Nodes.size() << endl;
    cout << "# of footways: " << Footways.size() << endl;
    cout << "# of buildings: " << Buildings.size() << endl;

    //
    // Add vertices:
    //
    for (auto& node : Nodes) {
        G.addVertex(node.first);
    }

    //
    // Add edges:
    //
    addEdge(G, Nodes, Footways);
   
    cout << "# of vertices: " << G.NumVertices() << endl;
    cout << "# of edges: " << G.NumEdges() << endl;
    cout << endl;

    //
    // Navigation from building to building
    //
    string startBuilding, destBuilding;

    cout << "Enter start (partial name or abbreviation), or #> ";
    getline(cin, startBuilding);

    bool foundStart, foundDest;
    long long startId, destId;
    BuildingInfo buildingStart, buildingDest;
    double startLat, startLong, destLat, destLong;
    while (startBuilding != "#")
    {
        cout << "Enter destination (partial name or abbreviation)> ";
        getline(cin, destBuilding);

        // Look for start building:
        foundStart = findBuilding(startBuilding, Buildings, buildingStart);

        if (!foundStart)
            cout << "Start building not found" << endl;
      
        else
        {
            // Look for destination building:
            foundDest = findBuilding(destBuilding, Buildings, buildingDest);

            if (!foundDest)
                cout << "Destination building not found" << endl;

            // Starting and destination buildings found, now search for path:
            else
            {
                // get info for start building:
                startId = buildingStart.Coords.ID;
                startLat = buildingStart.Coords.Lat;
                startLong = buildingStart.Coords.Lon;

                cout << "Starting point:" << endl;
                cout << " " << buildingStart.Fullname << endl;
                cout << " (" << startLat << ", " << startLong << ")" << endl;

                // get info for destination building:
                destId = buildingDest.Coords.ID;
                destLat = buildingDest.Coords.Lat;
                destLong = buildingDest.Coords.Lon;

                cout << "Destination point:" << endl;
                cout << " " << buildingDest.Fullname << endl;
                cout << " (" << destLat << ", " << destLong << ")" << endl;

                //
                // There is no direct path between buildings
                // We must search the nearest nodes (on a footpath) to the
                // start and destination building
                //
                findNearestNodes(startId, destId, startLat, startLong, destLat, destLong, Footways, Nodes);

                cout << "Nearest start node:" << endl;
                cout << " " << startId << endl;
                cout << " (" << startLat << "," << " " << startLong << ")" << endl;

                cout << "Nearest destination node:" << endl;
                cout << " " << destId << endl;
                cout << " (" << destLat << "," << " " << destLong << ")" << endl;

                // Use Dijkstra's algorithm to find the shortest path:
                cout << "Navigating with Dijkstra..." << endl;

                Dijkstra(G, startId, predecessors, distances);
                displayShortestPath(distances, predecessors, startId, destId);
            }
        }
       
        //
        // another navigation?
        //
        cout << endl;
        cout << "Enter start (partial name or abbreviation), or #> ";
        getline(cin, startBuilding);
    }

    //
    // done:
    //
    cout << "** Done **" << endl;

    return 0;
}
