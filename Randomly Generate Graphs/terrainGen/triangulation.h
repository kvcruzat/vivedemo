#ifndef TRIANGULATION_H
#define TRIANGULATION_H 1

#include "terrainGen.h"
#include "readTerrain.h"


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <time.h>
#include <vector>
#include <algorithm>
#include <cmath>
#include <string>

#include <math.h>
#include <utility>

//Global variables, change if needed
//SQUARE SIZE = number of coordiantes in each axis
//Needs to be (2^n)+1
const int SQUARE_SIZE = 1025;
const int MID_POINTS = SQUARE_SIZE / 500;
//Num of nodes inserted into graph, increase for morecomplex rivers
const int NUM_INSERT_POINTS = 100;
//Number of nodes used in end graph, increase to make game more challenging
const int NUM_NODES = 10;

const int RIVER_WIDTH = 25;

//Variables for 2nd method
const int NUM_POINTS = 10;

int insertPointAndFindTriangle(std::vector< std::vector<int > >* coordinates, std::vector< std::vector<int > > triangles);
float crossProduct(int p1x, int p1y, int p2x, int p2y, int p3x, int p3y);
bool pointInTriangle(int pTestx, int pTesty, int p1x, int p1y, int p2x, int p2y, int p3x, int p3y);
void connectTriangle(std::vector< std::vector<int> >* triangles, std::vector< std::vector<int> >* coordinates, int insideTriangle);
void printToFile(std::vector< std::vector<int> > triangles, std::vector< std::vector<int> > coordinates, std::vector< std::vector<float> > weightMatrix);

void findConnections(std::vector<std::vector<int> >* connections, std::vector<int> usedNodes, std::vector<std::vector<int> > coordinates);
std::vector< std::vector< std::vector<int> > > dijkstra(std::vector< std::vector<int> > connections, std::vector< std::vector<float> > weightMatrix, std::vector<int> usedNodes);
void connectionsPrint(std::vector< std::vector< std::vector<int> > > shortestPaths, std::vector< int > usedNodes, std::vector< std::vector<std::string> > rodIndex, std::vector< std::vector<std::string> > riverNames);
void findNewNodesAndRemoveEdges(std::vector< std::vector< std::vector<int> > > shortestPaths, std::vector<std::vector<int> > coordinates, std::vector< std::vector<int> > connections, std::vector< int > usedNodes, std::vector<std::vector<float> >* weightMatrix);
void printHeightMap(std::vector< std::vector<int> > connectionsMatrix, std::vector< std::vector<int> > coordinates, std::vector<int> usedNodes, std::vector<std::vector< std::vector<int > > > shortestPaths, std::vector<int> removeStart, std::vector<int> connectEnd);
// void drawLine(std::vector< std::vector<int> >*, std::vector<int>, std::vector<int>);
void drawLine(std::vector< std::vector<float> >* heightMap, int x1, int x2, int y1, int y2, int nodeIndex, std::vector< std::vector<float> >* rodLocations, std::vector< std::vector<int> >* riverLocations, std::vector<std::vector<std::string> >* rodindex, int node1, int node2, std::vector<std::string>* riverNames);
void makeDitches(std::vector< std::vector<float> >* oldHeightMap);

//Functions for 2nd method
std::vector<std::vector<int> > findNodes();
std::vector< std::vector<int> > addLinesToGraph();
void findWeightMatrix(std::vector< std::vector<int> >* graphConnections, std::vector< std::vector<int> > coordinates, std::vector< std::vector<float> >* weightMatrix);
void updateCoordinates(std::vector< std::vector<int> >* coordinates);

void outputNodes(std::vector<int> usedNodes, std::vector<std::vector<int> > coordinates);

std::vector<std::string> greedyShortestPath(std::string startRiver, std::vector<int> nodes, std::vector<std::string> riverNames, std::vector<std::string> startNodes, std::vector<std::string> endNodes);

#endif