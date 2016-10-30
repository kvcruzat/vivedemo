#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>

//NUM OF POINTS
const int SQUARE_SIZE = 1000;

int main(int argc, char* argv[])
{
	std::ofstream outputHeightMap;
	outputHeightMap.open("heightmaps/terrain.txt");

	for (int i = 0; i < SQUARE_SIZE; i++)
	{
		for (int j = 0; j < SQUARE_SIZE; j++)
		{
			outputHeightMap << ((2.0f * (float)SQUARE_SIZE) - ((float)i + (float)j)) /  (2.0f *(float)SQUARE_SIZE) << " ";
		}

		outputHeightMap << std::endl;
	}
}