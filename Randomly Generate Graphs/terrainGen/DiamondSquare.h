#ifndef DIAMONDSQUARE_H
#define DIAMONDSQUARE_H 1

//Class file to perform diamond square algorithm and generate random terrain, that looks
//Somehwat realistic

#include <math.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <time.h>
#include <stdlib.h>
#include <cstdlib>

class DiamondSquare
{
public:
	DiamondSquare(int power, float p0, float p1, float p2, float p3);

	int size;
	int maxValue;
	float **outputValues;
	float scale;

	float** getOutputValues();
	void outputValuesToTxt();

private:
	void runDiamondSquare(int size);
	void diamond(int x, int y, int size, float offset);
	void square(int x, int y, int size, float offset);

};

#endif