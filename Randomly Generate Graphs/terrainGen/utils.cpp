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

//NUM OF POINTS
const int SQUARE_SIZE = 1000;

utils::utils()
{

}

void utils::removeEmptyStrings(std::vector<std::string>& strings)
{
  std::vector<std::string>::iterator it = std::remove_if(strings.begin(), 
  	strings.end(),std::mem_fun_ref(&std::string::empty));
  // erase the removed elements
  strings.erase(it, strings.end());
}

std::vector<std::string> utils::split(std::string s, std::string delimiter) {
	size_t pos = 0;
	std::string token;
	std::vector<std::string> returnVector;
	while ((pos = s.find(delimiter)) != std::string::npos) {
    	token = s.substr(0, pos);
    	returnVector.push_back(token);
    	s.erase(0, pos + delimiter.length());
	}
	returnVector.push_back(s);
	removeEmptyStrings(returnVector);

	return returnVector;
}

//Function that reads in a terrain .txt file and outputs it as a model file
std::vector< std::vector< std::string > > utils::readTerrainFile(std::string filename) {
	std::string line;
	//Loads input file from parameters
	std::ifstream textFile(("../heightmaps/" + filename));

	//Vector for storing the split line
	std::vector<std::string> linevector;
	//Vector that stores all the terrain coordinates read in
	std::vector< std::vector<std::string> > terrainCoords;
	//Variables to keep track of amount of rows and lines in txt file
	int numLines = 0;
	int numCols = 0;

	//Check if file is open
	if (textFile.is_open())
	{	
		//Read line by line
		while (std::getline(textFile, line))
		{
			//Clear vector
			linevector.clear();
			//Split line
			linevector = split(line, " ");
			//Store split line in terraincoords vector
			terrainCoords.push_back(linevector);
			//Increment numLines
			numLines++;
		}
	}

	//Get number of columns from vector size
	numCols = linevector.size();
	//Close text file
	textFile.close();

	return terrainCoords;
}

void utils::outputTerrain(std::vector< std::vector<float> > terrainCoords)
{
	std::ofstream terrainFile(("../heightmaps/dsTerrain.txt"));

	//Vectors to hold coordinates
	std::vector<float> coordinates;

	int numLines = terrainCoords.size();
	int numCols = terrainCoords.size();
	//check if m file is open
	if(terrainFile.is_open())
	{
		for (int i = 0; i < numLines; i++)
		{
			for ( int j = 0; j < numCols; j++)
			{
				terrainFile << terrainCoords[i][j] << " ";
			}
			terrainFile << std::endl;
		}
	}
}