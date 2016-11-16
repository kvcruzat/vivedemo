#include "terrainGen.h"


terrainGen::terrainGen()
{

}

void terrainGen::generateTerrain(std::vector<std::vector<float> > *graphCoords)
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

	readTerrain::readTerrainFileAndOutputM(floatGraphCoords);
}