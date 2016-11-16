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
	static void generateTerrain(std::vector< std::vector<float> >*);
private:

};

#endif