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

//NUM OF POINTS
const int SQUARE_SIZE = 1025;

// int main(int argc, char* argv[])
// {
// 	std::ofstream outputHeightMap;
// 	outputHeightMap.open("heightmaps/terrain.txt");

// 	std::ifstream inputGraphMap;
// 	inputGraphMap.open

// 	for (int i = 0; i < SQUARE_SIZE; i++)
// 	{
// 		for (int j = 0; j < SQUARE_SIZE; j++)
// 		{
// 			outputHeightMap << ((2.0f * (float)SQUARE_SIZE) - ((float)i + (float)j)) /  (2.0f *(float)SQUARE_SIZE) << " ";
// 		}

// 		outputHeightMap << std::endl;
// 	}
// }

<<<<<<< HEAD
=======
// int main(int argc, char* argv[])
// {
// 	std::ofstream outputHeightMap;
// 	outputHeightMap.open("heightmaps/terrain.txt");

// 	std::ifstream inputGraphMap;
// 	inputGraphMap.open

// 	for (int i = 0; i < SQUARE_SIZE; i++)
// 	{
// 		for (int j = 0; j < SQUARE_SIZE; j++)
// 		{
// 			outputHeightMap << ((2.0f * (float)SQUARE_SIZE) - ((float)i + (float)j)) /  (2.0f *(float)SQUARE_SIZE) << " ";
// 		}

// 		outputHeightMap << std::endl;
// 	}
// }

>>>>>>> 63769324e99b1125701623cd50878b8806ce7b52
void removeEmptyStrings(std::vector<std::string>& strings)
{
  std::vector<std::string>::iterator it = std::remove_if(strings.begin(), 
  	strings.end(),std::mem_fun_ref(&std::string::empty));
  // erase the removed elements
  strings.erase(it, strings.end());
}

std::vector<std::string> split(std::string s, std::string delimiter) {
	size_t pos = 0;
	std::string token;
	std::vector<std::string> returnVector;
	while ((pos = s.find(delimiter)) != std::string::npos) {
    	token = s.substr(0, pos);
    	returnVector.push_back(token);
    	s.erase(0, pos + delimiter.length());
	}
	returnVector.push_back(s);

// 	std::vector<std::string>::iterator i = returnVector.begin();
// 	while(i != returnVector.end())
// 	{
//     	if(i->find('\0', 0) != std::string::npos)
//     	{
//         	i = returnVector.erase(i);
//     	}
//     	else
//     	{
//         	++i;
//     	}
// }
	removeEmptyStrings(returnVector);

	return returnVector;
}

//Function that reads in a terrain .txt file and outputs it as a model file
void readTerrainFileAndCombine() {
	std::string line;
	//Loads input file from parameters
	std::ifstream textFile(("heightmaps/graphHeightMap.txt"));
	//Create output file
	std::ofstream terrainFile("heightmaps/terrain.txt");

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
	//Vectors to hold coordinates and faces
	std::vector<float> coordinates;
	//check if m file is open
	if(terrainFile.is_open())
	{
		for(int i = 0; i < numLines; i++)
		{
			for(int j = 0; j < numCols; j++)
			{
				coordinates.push_back(atof(terrainCoords[i][j].c_str()) * (((2.0f * (float)SQUARE_SIZE) - ((float)i + (float)j)) /  (2.0f *(float)SQUARE_SIZE)));
			}
		}

		for (int i = 0; i < numLines; i++)
		{
			for ( int j = 0; j < numCols; j++)
			{
				terrainFile << coordinates[i + (j * numCols)] << " ";
			}
			terrainFile << std::endl;
		}
	}
}

int main (int argc, char *argv[])
{
	readTerrainFileAndCombine();

	return 1;
}
