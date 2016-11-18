#include "terrainGen.h"


terrainGen::terrainGen()
{

}

void terrainGen::generateTerrain(std::vector<std::vector<float> > *graphCoords, std::vector<int> *usedNodes, std::vector<std::vector<int> > *coordinates)
{
	// std::vector<std::vector<std::string> > graphCoords = utils::readTerrainFile("graphHeightMap.txt");

	int power = log2(graphCoords->size());

	DiamondSquare ds = DiamondSquare(power, 1.0f, 0.5f, 0.5f, 0.0f);

	float** dsCoords = ds.getOutputValues();

	std::vector<std::vector< float> > floatGraphCoords(graphCoords->size(), std::vector<float>(graphCoords->size(), 0.0f));

	for (unsigned i = 0; i < graphCoords->size(); i++)
	{
		for (unsigned j  = 0; j < graphCoords->size(); j++)
		{
			floatGraphCoords[i][j] = (*graphCoords)[i][j] - (1 - dsCoords[i][j]);
		}
	}
	// utils::outputTerrain(floatGraphCoords);

	outputNodes(*usedNodes, *coordinates, floatGraphCoords);

	readTerrain::readTerrainFileAndOutputM(floatGraphCoords);
}

void terrainGen::outputNodes(std::vector<int> usedNodes, std::vector<std::vector<int> > coordinates, std::vector<std::vector<float> > terrainCoords)
{
	std::ofstream nodeFile;
	//Temporary Value
	nodeFile.open("../../RiversofHanoi/Content/models/nodes.txt");

	std::cout << terrainCoords[0][0] << std::endl;

	for (int i = 0; i < usedNodes.size(); i++)
	{
		int x = coordinates[usedNodes[i]][0];
		int y = coordinates[usedNodes[i]][1];
		float z = terrainCoords[x][y];
		// std::cout << x << "," << y << "," << z << std::endl;
		nodeFile << x << "," << y << "," << z << std::endl;
	}
}