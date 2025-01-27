#ifndef TERRAINGEN_H
#define TERRAINGEN_H 1

#include <iostream>
#include "math.h"
#include "DiamondSquare.h"
#include "utils.h"
#include "readTerrain.h"
#include <stdlib.h>
#include <vector>

class terrainGen
{
public:
	terrainGen();
	static void generateTerrain(std::vector< std::vector<float> >* graphCoords, std::vector<int> *usedNodes, std::vector<std::vector<int> > *coordinates, std::vector<std::vector<float> > *rodLocations, std::vector<std::vector<int> > *riverLocations, int SQUARE_SIZE, std::vector<std::vector<std::string> > *rodIndex, std::vector<std::string> *riverNames);
private:
	static void outputNodes(std::vector<int> usedNodes, std::vector<std::vector<int> > coordinates, std::vector<std::vector<float> > terrainCoords, int SQUARE_SIZE);
	static void outputRods(std::vector<int> usedNodes, std::vector<std::vector<float> > rodLocations, std::vector<std::vector<float> > terrainCoords);
	static void outputRivers(std::vector<int> usedNodes, std::vector<std::vector<int> > riverLocations, float** terrainCoords, int SQUARE_SIZE, std::vector<std::string> riverNames);
	static void outputRodIndex(std::vector<std::vector<std::string> > rodIndex);

};

#endif