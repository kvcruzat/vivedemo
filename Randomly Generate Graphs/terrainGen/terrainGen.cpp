#include "terrainGen.h"
<<<<<<< HEAD
#include "DiamondSquare.h"
#include "utils.h"

int main(int argc, char* argv[])
{
	std::vector<std::vector<std::string> > graphCoords = utils::readTerrainFile("graphHeightMap.txt");

	int power = log2(graphCoords.size());

	DiamondSquare ds = DiamondSquare(power, 1.0f, 0.5f, 0.5f, 0.0f);

	float** dsCoords = ds.getOutputValues();

	std::vector<std::vector< float> > floatGraphCoords(graphCoords.size(), std::vector<float>(graphCoords.size(), 0.0f));

	for (unsigned i = 0; i < graphCoords.size(); i++)
	{
		for (unsigned j  = 0; j < graphCoords.size(); j++)
		{
			floatGraphCoords[i][j] = atof(graphCoords[i][j].c_str()) - (1 - dsCoords[i][j]);
		}
	}
	utils::outputTerrain(floatGraphCoords);
=======

int main(int argc, char[] *argv)
{

>>>>>>> 63769324e99b1125701623cd50878b8806ce7b52
}