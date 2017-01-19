#ifndef READTERRAIN_H
#define READTERRAIN_H 1

#include <vector>
#include <math.h>
#include <string>
#include <string.h>
#include <sstream>
#include <iostream>
#include <iterator>
#include <fstream>
#include <stdlib.h>
#include <cstdlib>
#include <algorithm>
#include <time.h>
#include <iomanip>

#include "utils.h"

class readTerrain
{
public:
	readTerrain();
	static void removeEmptyStrings(std::vector<std::string>& strings);
	static std::vector<std::string> split(std::string s, std::string delimiter);
	static int readTerrainFileAndOutputM(std::vector <std::vector<float> > terrainCoords);
private:
};

#endif