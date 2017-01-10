#include "readTerrain.h"

readTerrain::readTerrain()
{

}

//Function that reads in a terrain .txt file and outputs it as a model file
int readTerrain::readTerrainFileAndOutputM(std::vector <std::vector<float> > terrainCoords) {
	std::string outputLoc = "../../RiversofHanoi/Content/models/terrain.m";
	// std::string line;
	// //Loads input file from parameters
	// std::ifstream textFile((fileName).c_str());
	//Create output file
	std::ofstream modelFile(outputLoc);

	//Vector for storing the split line
	// std::vector<std::string> linevector;
	//Vector that stores all the terrain coordinates read in
	// std::vector< std::vector<std::string> > terrainCoords;
	//Variabels to keep track of amount of rows and lines in txt file
	int numLines = 0;
	int numCols = 0;

	//Check if file is open
	// if (textFile.is_open())
	// {	
	// 	//Read line by line
	// 	while (std::getline(textFile, line))
	// 	{
	// 		//Clear vector
	// 		linevector.clear();
	// 		//Split line
	// 		linevector = utils::split(line, " ");
	// 		//Store split line in terraincoords vector
	// 		terrainCoords.push_back(linevector);
	// 		//Increment numLines
	// 		numLines++;
	// 	}
	// }

	//Get number of columns from vector size
	numLines = terrainCoords.size();
	numCols = terrainCoords.size();

	// linevector.clear();
	// linevector.shrink_to_fit();
	//Close text file
	// textFile.close();
	//Vectors to hold vertices and faces
	// std::vector<float> vertices;
	// std::vector<int> indices;
	std::cout << "Making vectors" << std::endl;

	//check if m file is open
	if(modelFile.is_open())
	{
		//variables to hold indices of faces
		int p1Index = 0;
		int p2Index = 0;
		int p3Index = 0;
		int p4Index = 0;

		float point1;
		float point2;
		float point3;

		std::cout << "Vertices" << std::endl;

		std::cout << "Writing vertices to file" << std::endl;
		//Write comments to file
		modelFile << "#" << std::endl << "# Created by Philip Nilsson" << 
		std::endl << "#" << std::endl;
		modelFile << "#" << std::endl << "# Terrain vertices=" << (numLines * numCols) <<
		 " faces=" << (numLines - 1) * (numCols - 1) * 2 << std::endl;

		//Loop through entire file
		for(int i = 0; i < numLines; i++)
		{
			for(int j = 0; j < numCols; j++)
			{
				//Add vertex to vector with y value of terrain txt
				int index = ((i * j) + j);
				point1 = (j - (numCols / 2)) / (float)(numLines / 2.00f);
				point2 = terrainCoords[i][j];
				point3 = (i - (numLines / 2)) / (float)(numCols / 2.00f);

				modelFile << "Vertex " << index << " " << point1 << " " << 
					point2 << " " << point3  << std::endl;

				// vertices.push_back((j - (numCols / 2)) / (float)(numLines / 2.00f));
				// vertices.push_back(atof(terrainCoords[i][j].c_str()));
				// vertices.push_back((i - (numLines / 2)) / (float)(numCols / 2.00f));
			}
		}

		terrainCoords.clear();
		terrainCoords.shrink_to_fit();
		// vertices.size() << " faces=" << (numLines - 1) * (numCols - 1) * 2 << std::endl;

		// //Loop through and write vertices to file
		// for (int i = 0; i < vertices.size() / 3; i++)
		// {
		// 	modelFile << "Vertex " << i << " " << vertices[3 * i] << " " << 
		// 	vertices[(3 * i) + 1] << " " << vertices[(3 * i) + 2]  << std::endl;
		// }

		// vertices.clear();
		// vertices.shrink_to_fit();

		std::cout << "Computing Indices" << std::endl;

		for(int i = 0; i < (numLines - 1); i++)
		{
			for(int j = 0; j < (numCols - 1); j++)
			{
				try
				{
					//Find indices of vertices needed
					p1Index = ((i * numCols) + j);
					p2Index = ((i * numCols) + j) + 1;
					p3Index = ((i * numCols) + j) + numCols;
					p4Index = ((i * numCols) + j) + 1 + numCols;

					int index = ((i * j) + j) * 2;

					modelFile << "Face " << index << " " << p1Index << " " << 
						p3Index << " " << p4Index  << std::endl;

					modelFile << "Face " << index + 1 << " " << p1Index << " " << 
						p4Index << " " << p2Index << std::endl;  
				} catch (std::bad_alloc& ba)
				{
					std::cout << "Caught bad_alloc" << std::endl;
				}
			}
		}
	}

	return 1;
}
