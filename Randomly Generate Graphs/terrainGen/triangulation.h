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

#include <math.h>
#include <utility>

//Global variables, change if needed
//SQUARE SIZE = number of coordiantes in each axis
//Needs to be (2^n)+1
const int SQUARE_SIZE = 2049;
const int MID_POINTS = SQUARE_SIZE / 500;
//Num of nodes inserted into graph, increase for morecomplex rivers
const int NUM_INSERT_POINTS = 100;
//Number of nodes used in end graph, increase to make game more challenging
const int NUM_NODES = 10;

//Variables for 2nd method
const int NUM_POINTS = 10;

int insertPointAndFindTriangle(std::vector< std::vector<int > >*, std::vector< std::vector<int > >);
float crossProduct(int, int, int, int, int, int);
bool pointInTriangle(int, int, int, int, int, int, int, int);
void connectTriangle(std::vector< std::vector<int> >*, std::vector< std::vector<int> >*, int);
void printToFile(std::vector< std::vector<int> >, std::vector< std::vector<int> >, std::vector< std::vector<float> >);

void findConnections(std::vector<std::vector<int> >*, std::vector<int>, std::vector<std::vector<int> >);
std::vector< std::vector< std::vector<int> > > dijkstra(std::vector< std::vector<int> >, std::vector< std::vector<float> >, std::vector<int>);
void testPrint(std::vector< std::vector< std::vector<int> > >, std::vector< int >, std::vector< std::vector<int> >);
void printGraph(std::vector< std::vector< std::vector<int> > >, std::vector<std::vector<int> > , std::vector< std::vector<int> >, std::vector< int >);
void printHeightMap(std::vector< std::vector<int> > , std::vector< std::vector<int> >);
// void drawLine(std::vector< std::vector<int> >*, std::vector<int>, std::vector<int>);
void drawLine(std::vector< std::vector<float> >*, int, int, int, int);
void makeDitches(std::vector< std::vector<float> >*);

//Functions for 2nd method
std::vector<std::vector<int> > findNodes();
std::vector< std::vector<int> > addLinesToGraph();
void findWeightMatrix(std::vector< std::vector<int> >*, std::vector< std::vector<int> >, std::vector< std::vector<float> >*);
void updateCoordinates(std::vector< std::vector<int> >*);

void outputNodes(std::vector<int>, std::vector<std::vector<int> >);

#endif