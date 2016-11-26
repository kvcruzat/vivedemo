#include "terrainGen.h"


terrainGen::terrainGen()
{

}

void terrainGen::generateTerrain(std::vector<std::vector<float> > *graphCoords, std::vector<int> *usedNodes, std::vector<std::vector<int> > *coordinates, std::vector<std::vector<float> > *rodLocations, std::vector<std::vector<int> > *riverLocations, int SQUARE_SIZE)
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
			//floatGraphCoords[i][j] = (*graphCoords)[i][j] - (1 - dsCoords[i][j]);
			floatGraphCoords[i][j] = dsCoords[i][j] - (0.05 * (1 - (*graphCoords)[i][j]));
		}
	}
	// utils::outputTerrain(floatGraphCoords);

	outputNodes(*usedNodes, *coordinates, floatGraphCoords);

	outputRods(*usedNodes, *rodLocations, floatGraphCoords);

	outputRivers(*usedNodes, *riverLocations, floatGraphCoords, SQUARE_SIZE);

	readTerrain::readTerrainFileAndOutputM(floatGraphCoords);
}

void terrainGen::outputNodes(std::vector<int> usedNodes, std::vector<std::vector<int> > coordinates, std::vector<std::vector<float> > terrainCoords)
{
	std::ofstream nodeFile;
	nodeFile.open("../../RiversofHanoi/Content/models/nodes.txt");

	for (int i = 0; i < usedNodes.size(); i++)
	{
		int x = coordinates[usedNodes[i]][0];
		int y = coordinates[usedNodes[i]][1];
		float z = terrainCoords[x][y];
		// std::cout << x << "," << y << "," << z << std::endl;
		nodeFile << x << "," << y << "," << z << std::endl;
	}
}

void terrainGen::outputRods(std::vector<int> usedNodes, std::vector<std::vector<float> > rodLocations, std::vector<std::vector<float> > terrainCoords)
{
	std::ofstream rodFile;
	rodFile.open("../../RiversofHanoi/Content/models/rods.txt");

	for (int i = 0; i < rodLocations.size(); i++)
	{
		for (int j = 0; j < rodLocations[i].size(); j+=2)
		{
			int x = rodLocations[i][j];
			int y = rodLocations[i][j+1];
			float z = terrainCoords[x][y];
			rodFile << x << "," << y << "," << z << " ";
		}

		rodFile << std::endl;
	}
}

void terrainGen::outputRivers(std::vector<int> usedNodes, std::vector<std::vector<int> > riverLocations, std::vector<std::vector<float> > terrainCoords, int SQUARE_SIZE)
{
	std::ofstream riverFile;
	riverFile.open("../../RiversofHanoi/Content/models/rivers.txt");

	// std::cout << riverLocations.size() << std::endl;
	// std::cout << riverLocations[0].size() << std::endl;

	for (int i = 0; i < riverLocations.size(); i++)
	{
		for (int j = 0; j < riverLocations[i].size(); j+=2)
		{
			int x = riverLocations[i][j];
			int y = riverLocations[i][j+1];

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
			float z = terrainCoords[x][y];
			riverFile << x << "," << y << "," << z << " ";
		}

		riverFile << std::endl;
	}
}