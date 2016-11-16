#include "DiamondSquare.h"

//Constructor
DiamondSquare::DiamondSquare(int power, float p0, float p1, float p2, float p3)
{
	//Set random seed
	srand(time(0));
	//Set size to be 2^(input) + 1
	size = pow(2, power) + 1;
	//Set max index
	maxValue = size - 1;
	//Init array
	outputValues = new float*[size];
	for (int i = 0; i < size; i++)
	{
		outputValues[i] = new float[size];
	}

	//Init Corner points as random points
	outputValues[0][0] =  p0; //((float)rand() / RAND_MAX);
	outputValues[0][maxValue] = p1; //((float)rand() / RAND_MAX);
	outputValues[maxValue][0] = p2; //((float)rand() / RAND_MAX);
	outputValues[maxValue][maxValue] = p3; //((float)rand() / RAND_MAX);

	//Set scale, higher scale, less smooth terrain
	//Ideally less than 1.0f
	//But can go as high as you please
	scale = 0.3f;

	//Run diamond square
	runDiamondSquare(maxValue);
	//Output values to terrain.txt
	// outputValuesToTxt(); 
}

void DiamondSquare::runDiamondSquare(int size)
{
	//Set x y and halfStep to be size/2
	int x = size / 2;
	int y = size / 2;
	int halfStep = size / 2;

	//Check if algorithm has finished
	if (halfStep < 1)
	{
		return;
	}

	//Perform all diamond steps
	for ( y = halfStep; y < maxValue; y += size)
	{
		for (x = halfStep; x < maxValue; x += size)
		{
			diamond(x, y, halfStep, (((float)rand() / RAND_MAX) - 0.5f)
				* scale);
		}
	}

	//Perform all square steps
	for ( y = 0; y <= maxValue; y += halfStep)
	{
		for (x = (y + halfStep) % size; x <= maxValue; x += size)
		{
			square(x, y, halfStep, (((float)rand() / RAND_MAX) - 0.5f) 
				* scale);
		}
	}
	// for ( y = halfStep; y <= maxValue; y += halfStep)
	// {
	// 	for (x = (y + halfStep) % size; x <= maxValue; x += size)
	// 	{
	// 		if (y < 5)
	// 		{
	// 			std::cout << y << std::endl;
	// 		}
	// 		square(x, y, halfStep, (((float)rand() / RAND_MAX) - 0.5f) 
	// 			* scale);
	// 	}
	// }

	scale /= 2.0f;
	runDiamondSquare(size / 2);
}

//Function that performs diamond step
void DiamondSquare::diamond(int x, int y, int size, float offset)
{
	//Average variable
	float average;

	//Gets average from 4 corners around in
	//Distance size away
	average = (outputValues[x - size][y - size] +
		outputValues[x + size][y - size] +
		outputValues[x - size][y + size] +
		outputValues[x + size][y + size]) / 4;

	//Sets point to average plus some noise
	//To get a rougher terrain
	outputValues[x][y] = (average + offset);
}

//Function that performs sqauare step
void DiamondSquare::square(int x, int y, int size, float offset)
{
	float average;
	//Need to check if point is on edge to not seg fault

	//Checks if point is on left edge
	if ( (x - size) < 0)
	{
		//Uses point from above, below and right
		average = (outputValues[x][y - size] +
			outputValues[x + size][y] +
			outputValues[x][y + size]) / 3;
	} 
	//Checks if on right edge
	else if ((x + size) > maxValue)
	{
		//Uses point from left, up and below
		average = (outputValues[x][y - size] +
			outputValues[x][y + size] +
			outputValues[x - size][y]) / 3;
	} 
	//Checks if on top edge
	else if((y - size) < 0)
	{
		//Uses points from right, left and below
		average = (outputValues[x + size][y] +
			outputValues[x][y + size] +
			outputValues[x - size][y]) / 3;
	} 
	//Checks if on bottom edge
	else if((y + size) > maxValue)
	{
		//Uses points from right, left, above
		average = (outputValues[x][y - size] +
			outputValues[x + size][y] +
			outputValues[x - size][y]) / 3;
	} 
	//Otherwise
	else {
		//Uses points from right, left, above and below
		average = (outputValues[x][y - size] +
			outputValues[x + size][y] +
			outputValues[x][y + size] +
			outputValues[x - size][y]) / 4;
	}

	//Sets point to average plus noise value
	outputValues[x][y] = (average + offset);
}


//Function to output values to .txt file
void DiamondSquare::outputValuesToTxt()
{
	//Set ofstream file
	std::ofstream terrainFile("test/terrain.txt");

	//Loop through each value in array
	for ( int i = 0; i <= maxValue; i++)
	{
		for (int j = 0; j <= maxValue; j++)
		{
			//First point in terrain array
			if ( j == 0 && i == 0)
			{
				//Output with a space
				terrainFile << std::setprecision(3) << std::fixed << 
				outputValues[i][j] << " ";
			}
			//If in last row and last column
			else if ( i == (maxValue) && j == (maxValue))
			{
				//Output with no space, as last value
				terrainFile << std::setprecision(3) << std::fixed << 
				outputValues[i][j];
			}
			//If in last column
			else if (j == (maxValue))
			{
				//Output with a linebreak
				terrainFile << std::setprecision(3) << std::fixed << 
				outputValues[i][j] << std::endl;
			}
			else{
				terrainFile << std::setprecision(3) << std::fixed << 
				outputValues[i][j] << " ";
			}
		}
	}
}

float** DiamondSquare::getOutputValues()
{
	return this->outputValues;
}