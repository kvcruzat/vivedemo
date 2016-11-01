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

#include <math.h>
#include <utility>

const int SQUARE_SIZE = 1000;
const int NUM_INSERT_POINTS = 100;
const int NUM_NODES = 10;

int insertPointAndFindTriangle(std::vector< std::vector<int > >*, std::vector< std::vector<int > >);
float crossProduct(int, int, int, int, int, int);
bool pointInTriangle(int, int, int, int, int, int, int, int);
void connectTriangle(std::vector< std::vector<int> >*, std::vector< std::vector<int> >*, int);
void printToFile(std::vector< std::vector<int> >, std::vector< std::vector<int> >, std::vector< std::vector<int> >);
void findWeightMatrix(std::vector< std::vector<int> >, std::vector< std::vector<int> >, std::vector< std::vector<int> >*);
void findConnections(std::vector<std::vector<int> >*);
std::vector< std::vector< std::vector<int> > > dijkstra(std::vector< std::vector<int> >, std::vector< std::vector<int> >, std::vector<int>);
void testPrint(std::vector< std::vector< std::vector<int> > >, std::vector< int >, std::vector< std::vector<int> >);
void printGraph(std::vector< std::vector< std::vector<int> > >, std::vector<std::vector<int> > , std::vector< std::vector<int> >, std::vector< int >);
void printHeightMap(std::vector< std::vector<int> > , std::vector< std::vector<int> >);
// void drawLine(std::vector< std::vector<int> >*, std::vector<int>, std::vector<int>);
void drawLine(std::vector< std::vector<float> >*, int, int, int, int);
void makeDitches(std::vector< std::vector<float> >*);