#ifndef UTILS_H
#define UTILS_H 1

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

class utils
{
public:
	utils();
	static std::vector< std::vector< std::string > > readTerrainFile(std::string);
	static void outputTerrain(std::vector< std::vector<float> > terraincoords);

private:
	static void removeEmptyStrings(std::vector<std::string>& strings);
	static std::vector<std::string> split(std::string s, std::string delimiter);
};

#endif