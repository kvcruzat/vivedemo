#include "terrainGen.h"

//Constructer
terrainGen::terrainGen()
{

}

/* Function to output all the information that needs to be output
 * Input -
 * *graphCoords - A matrix of size (2^n) - 1 with values either 0 or 1. This is the heightmap for the graph
 * *usedNodes - An array containing which nodes were the main nodes in the graph
 * *coordinates - A 2D array containing all the coordinates of the nodes
 * *rodLocations - A pointer to a 2D array containing the coordinates of each river
 * SQUARE_SIZE - A constant that stores the width and height of the terrain
 * *rodIndex - A pointer to a 3D array containing the name of each rod, and which node the rod belongs to
 * *riverNames - A pointer to a 2D containing the names of the rivers that make up one long river
 */
void terrainGen::generateTerrain(std::vector<std::vector<float> > *graphCoords,
 std::vector<int> *usedNodes,
  std::vector<std::vector<int> > *coordinates,
   std::vector<std::vector<float> > *rodLocations,
    std::vector<std::vector<int> > *riverLocations,
     int SQUARE_SIZE,
      std::vector< std::vector<std::string> > *rodIndex,
       std::vector<std::string> *riverNames)
{
	//Find which power the size of the graph is
	//This is needed for the Diamond Square algorithm
	int power = log2(graphCoords->size());

	//Run the diamond square algorithm, using preset heights for the corners
	DiamondSquare ds = DiamondSquare(power, 1.0f, 0.6f, 0.4f, 0.0f);

	//Get the output values from the diamond square algorithm
	float** dsCoords = ds.getOutputValues();

	//Make a new matrix the same size as graphCoords
	//This is used to store the new height map
	std::vector<std::vector< float> > floatGraphCoords(graphCoords->size(), std::vector<float>(graphCoords->size(), 0.0f));

	//Loop through the graph coords
	for (unsigned i = 0; i < graphCoords->size(); i++)
	{
		for (unsigned j  = 0; j < graphCoords->size(); j++)
		{
			//floatGraphCoords[i][j] = (*graphCoords)[i][j] - (1 - dsCoords[i][j]);
			//New coordinates are a combination of the diamond square algorithm
			//And the graph coords
			//Graph coords are scaled down so that the rivers are not too deep
			floatGraphCoords[i][j] =  dsCoords[i][j] - (0.05 * (1 - (*graphCoords)[i][j]));
		}
	}
	// utils::outputTerrain(floatGraphCoords);

	//Outputs various data to files
	outputNodes(*usedNodes, *coordinates, floatGraphCoords, SQUARE_SIZE);

	outputRods(*usedNodes, *rodLocations, floatGraphCoords);

	outputRivers(*usedNodes, *riverLocations, dsCoords, SQUARE_SIZE, *riverNames);

	outputRodIndex(*rodIndex);

	//Outputs the height map as a model file
	readTerrain::readTerrainFileAndOutputM(floatGraphCoords);
}

// Function to handle outputting the node data to the correct file
void terrainGen::outputNodes(std::vector<int> usedNodes, std::vector<std::vector<int> > coordinates, std::vector<std::vector<float> > terrainCoords, int SQUARE_SIZE)
{
	//Create an output stream
	std::ofstream nodeFile;
	//Open the correct file in the output stream
	nodeFile.open("../../RiversofHanoi/Content/models/nodes.txt");

	//Loop to add the top-left node and the bottom-right
	//These are needed to scale terrain in Unreal Engine
	for(int i = 0; i < 2; i++)
	{
		//Find the x coord
		int x = i * (SQUARE_SIZE - 1);
		//Find the y coord
		int y = i * (SQUARE_SIZE - 1);
		//Use x and y coord to look in array for z coord
		float z = terrainCoords[x][y];
		nodeFile << x << "," << y << "," << z << std::endl;
	}

	// std::cout << usedNodes.size() << std::endl;

	//Loop through all the used nodes
	for (int i = 0; i < usedNodes.size(); i++)
	{
		//Find the x coord
		int x = coordinates[usedNodes[i]][0];
		//Find the y coord
		int y = coordinates[usedNodes[i]][1];
		//Use x and y coord to look in array for z coord
		float z = terrainCoords[x][y];
		//Output point to file
		nodeFile << x << "," << y << "," << z << std::endl;
	}
}

// Function to handle outputting the rod data to the correct file
void terrainGen::outputRods(std::vector<int> usedNodes, std::vector<std::vector<float> > rodLocations, std::vector<std::vector<float> > terrainCoords)
{
	//Create an output stream
	std::ofstream rodFile;
	// Open the coorect file in the output stream
	rodFile.open("../../RiversofHanoi/Content/models/rods.txt");

	//add two blank rows for the first two fake nodes
	for (int i = 0; i < 2; i++)
	{
		rodFile << std::endl;
	}

	//Loop through all the rod locations
	for (int i = 0; i < rodLocations.size(); i++)
	{
		//Steps 2 as in the array the x and y coordinates are consectutive points in the array
		for (int j = 0; j < rodLocations[i].size(); j+=2)
		{
			//Find x coord in the array
			int x = rodLocations[i][j];
			//Find y coord in the array
			int y = rodLocations[i][j+1];
			//Use x and y coord to look in array for z coord
			float z = terrainCoords[x][y];
			//Output coordinates of the rod
			rodFile << x << "," << y << "," << z << " ";
		}
		//Add a new line, as it is had looped through all rods at that node
		rodFile << std::endl;
	}
}

// Function to handle outputting the river coordinates to the coorect file
void terrainGen::outputRivers(std::vector<int> usedNodes, std::vector<std::vector<int> > riverLocations, float** terrainCoords, int SQUARE_SIZE, std::vector<std::string> riverNames)
{
	//Open an output stream
	std::ofstream riverFile;
	//Open the the correct file with the output stream
	riverFile.open("../../RiversofHanoi/Content/models/rivers.txt");

	// std::cout << riverLocations.size() << std::endl;
	// std::cout << riverLocations[0].size() << std::endl;

	//Loop through the river locations array
	for (int i = 0; i < riverLocations.size(); i++)
	{
		// Variables used to store the highest value of the top and bottom of the river
		// These are used so that the river rectangle is not skewed when a point is on the edge of the graph
		float maxZTop = -1;
		float maxZBot = -1;
		//Loop through the array for one river
		//This is done to find what the variables maxZTop and maxZBot should be
		for (int j = 0; j < riverLocations[i].size(); j+=2)
		{
			//Find the x and y coords of one corner of the river
			int x = riverLocations[i][j];
			int y = riverLocations[i][j+1];

			//Check if any of the coordinates are out of bounds of the array
			//If they are out of bounds, set them to the closest point inside the array
			if (x < 0)
			{
				x = 0;
			} else if (x > (SQUARE_SIZE - 1))
			{
				x = SQUARE_SIZE - 1;
			}
			if (y < 0)
			{
				y = 0;
			} else if (y > (SQUARE_SIZE - 1))
			{
				y = SQUARE_SIZE - 1;
			}
			//Find the z value of the river corner
			float z = terrainCoords[x][y];

			//Check if j < 4
			//This is done as these are the top coords
			// And if j >= 4 then it is the bot coords
			if (j < 4)
			{
				//Check to see if the z coord is bigger than the max one.
				if (maxZTop < z)
				{
					//If it is then replace maxZTop
					maxZTop = z;
					// std::cout << maxZ << std::endl;
				}
			} else{
				//Check to see if the z coord is bigger than the max one.
				if (maxZBot < z)
				{
					//If it is then replace maxZBot
					maxZBot = z;
					// std::cout << maxZ << std::endl;
				}
			}
		}
		//Loop through the array again, this time to output the point
		for (int j = 0; j < riverLocations[i].size() - 1; j+=2)
		{
			//Find the x and y coordinates from array
			int x = riverLocations[i][j];
			int y = riverLocations[i][j+1];

			//Init z coord
			float z;

			//Check if any of the coordinates are out of bounds of the array
			//If they are out of bounds, set them to the closest point inside the array
			if (x < 0)
			{
				x = 0;
			} else if (x > (SQUARE_SIZE - 1))
			{
				x = SQUARE_SIZE - 1;
			}
			if (y < 0)
			{
				y = 0;
			} else if (y > (SQUARE_SIZE - 1))
			{
				y = SQUARE_SIZE - 1;
			}
			//Check if point is on edge of graph
			if(x == 0 || x == SQUARE_SIZE-1 || y == 0 || y == SQUARE_SIZE-1)
			{
				//If it is on edge of graph then the z value has to be replaced.
				if (j < 4)
				{
					z = maxZTop;
				} else {
					z = maxZBot;
				}
				// z = maxZ;

				// std::cout << maxZ << std::endl;
				// std::cout << "z = " << z << std::endl;

			} else {
				//If it is not then just look in terrain coords for z value
				z = terrainCoords[x][y];
			}
			//Output coordinate to file
			riverFile << x << "," << y << "," << z << " ";
		}

		//After coordinates are output, then the name of the river is output
		riverFile << riverNames[i];

		riverFile << std::endl;
	}
}

//Function that handles outputting the river that a rod is connnected to
void terrainGen::outputRodIndex(std::vector<std::vector<std::string> > rodIndex)
{
	//Init an output stream
	std::ofstream rodIndexFile;
	//Open the correct file in the output stream
	rodIndexFile.open("../../RiversofHanoi/Content/models/rodIndex.txt");

	//add two blank rows for the first two fake nodes
	for (int i = 0; i < 2; i++)
	{
		rodIndexFile << std::endl;
	}

	//Loop through the rod index array and output the values inside of it
	for (int i = 0; i < rodIndex.size(); i++)
	{
		for (int j = 0; j < rodIndex[i].size(); j++)
		{
			rodIndexFile << rodIndex[i][j] << " ";
		}

		rodIndexFile << std::endl;
	}
}