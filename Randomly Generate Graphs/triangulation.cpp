// #include <stdlib.h>
// #include <stdio.h>
// #include <string.h>
// #include <stdarg.h>
// #include <iostream>
// #include <fstream>
#include "triangulation.h"


// Uses a 1000 x 1000 grid
//Inserts 100 points into grid
//Picks 4 maxima points/4 minima points / 2 others
//Generates the graph

int main (int argc, char *argv[])
{
	srand (time(NULL));

	//Initialisea 100x100 matrix to be all zeros
	std::vector<std::vector<int> > triangles;
	std::vector<std::vector<int> > coordinates;

	//start with corners
	// (0,0) is top left
	coordinates.push_back(std::vector<int>{0, 0});
	coordinates.push_back(std::vector<int>{0, SQUARE_SIZE});
	coordinates.push_back(std::vector<int>{SQUARE_SIZE, 0});
	coordinates.push_back(std::vector<int>{SQUARE_SIZE, SQUARE_SIZE});

	triangles.push_back(std::vector<int>{0, 1, 3});
	triangles.push_back(std::vector<int>{0, 2, 3});



	for (int i = 0; i < NUM_INSERT_POINTS; i++)
	{
		int triangle = insertPointAndFindTriangle(&coordinates, triangles);
		connectTriangle(&triangles, &coordinates, triangle);
	}

	std::vector<std::vector<int> > weightMatrix(coordinates.size(), std::vector<int> (coordinates.size(), 0));

	findWeightMatrix(triangles, coordinates, &weightMatrix);

	//Picks random points to use as nodes
	std::vector< std::vector<int> > connections =  findConnections();

	std::vector< int > usedNodes;

	//Use top left and bottom right
	usedNodes.push_back(0);
	usedNodes.push_back(3);

	for (int i = 0; i < NUM_NODES - 2; i++)
	{
		int randNode = (rand() % (NUM_INSERT_POINTS + 4));
		while(std::find(usedNodes.begin(), usedNodes.end(), randNode) != usedNodes.end()) {
    		randNode = (rand() % (NUM_INSERT_POINTS + 4));
    	}

    	usedNodes.push_back(randNode);
	}

	std::vector< std::vector< std::vector<int> > > shortestPaths = dijkstra(connections, weightMatrix, usedNodes);

	printToFile(triangles, coordinates, weightMatrix);
	testPrint(shortestPaths, usedNodes);
	printGraph(shortestPaths, coordinates, connections);
}

void testPrint(std::vector< std::vector< std::vector<int> > > shortestPaths, std::vector< int > usedNodes)
{
	std::ofstream outputHeightMap;
	outputHeightMap.open("paths.txt");

	// int index = 0;

	// std::cout << "Height: " << height << " " << "Width: " << width << " " << "vector Length: " << imageOnlyGrey.size() << std::endl; 

	for ( unsigned i = 0; i < shortestPaths.size(); i++)
	{
		for ( unsigned j = 0; j < shortestPaths[i].size(); j++)
		{
			outputHeightMap << usedNodes[i] << " to " << j << " : ";
			for (unsigned k = 0; k < shortestPaths[i][j].size(); k++)
			{
				outputHeightMap << shortestPaths[i][j][k] << " ";
			}

			outputHeightMap << std::endl;
		}
		// std::cout << "i: " << i << std::endl;


	}
}

void printGraph(std::vector< std::vector< std::vector<int> > > shortestPaths, std::vector<std::vector<int> > coordinates, std::vector< std::vector<int> > connections)
{
	std::ofstream outputGraph;
	outputGraph.open("graphs.txt");


	std::vector<std::vector<int> > connectionsMatrix( shortestPaths[0].size(), std::vector<int> (shortestPaths[0].size(), 0));
	for (unsigned i = 0; i < connections.size(); i++)
	{

		for (unsigned j = 0; j < connections[i].size(); j++)
		{
			//int lastConnection = shortestPaths[i][j][0];
			std::vector<int> *ptrShortPath = &shortestPaths[i][connections[i][j]];
			for(unsigned k = 0; k < shortestPaths[i][connections[i][j]].size() - 1; k++)
			{
				connectionsMatrix[(*ptrShortPath)[k]][(*ptrShortPath)[k+1]] = 1;
			}
		}
	}


	for (unsigned i = 0; i < connectionsMatrix.size(); i++)
	{
		for (unsigned j = 0; j < connectionsMatrix[i].size(); j++)
		{
			outputGraph << connectionsMatrix[i][j] << " ";
		}

		outputGraph << std::endl;
	}

	outputGraph << std::endl << std::endl;

	for (unsigned i = 0; i < coordinates.size(); i++)
	{
		outputGraph << coordinates[i][0] << ", " << coordinates[i][1] << std::endl;
	}

	printHeightMap(connectionsMatrix, coordinates);
}

void printHeightMap(std::vector< std::vector<int> > connectionsMatrix, std::vector< std::vector<int> > coordinates)
{
	//Canal height map
	//iF 1 then canal is present
	std::vector< std::vector<int> > heightMap(SQUARE_SIZE, std::vector<int> (SQUARE_SIZE, 0));

	// std::cout << connectionsMatrix.size() << std::endl;

	for (unsigned i = 0; i < connectionsMatrix.size(); i++)
	{
		// std::cout << connectionsMatrix[i].size() << std::endl;
		for (unsigned j = 0; j < connectionsMatrix[i].size(); j++)
		{
			if ( connectionsMatrix[i][j] != 0)
			{
				//Bresenham's Line algorithm
				// std::cout << i << " " << j << std::endl;
				std::cout << coordinates[i][0] << " " << coordinates[i][1] << std::endl;
				std::cout << coordinates[j][0] << " " << coordinates[j][1] << std::endl << std::endl;
				// drawLine(&heightMap, coordinates[i], coordinates[j]);
				drawLine(&heightMap, coordinates[i][0], coordinates[j][0], coordinates[i][1], coordinates[j][1]);
			}
		}
	}

	std::ofstream graphHeightMap;
	graphHeightMap.open("heightmaps/graphHeightMap.txt");

	for (unsigned i = 0; i < heightMap.size(); i++)
	{
		for (unsigned j = 0; j < heightMap[i].size(); j++)
		{
			graphHeightMap << heightMap[i][j] << " ";
		}

		graphHeightMap << std::endl;
	}
}

// void drawLine(std::vector< std::vector<int> > *heightMap, std::vector<int> startPoint, std::vector<int> endPoint)
void drawLine(std::vector< std::vector<int> > *heightMap, int x1, int x2, int y1, int y2)
{
	//
	// Bresenham's line algorithm
	const bool steep = (fabs(y2 - y1) > fabs(x2 - x1));
	if(steep)
	{
	  std::swap(x1, y1);
	  std::swap(x2, y2);
	}
	
	if(x1 > x2)
	{
	  std::swap(x1, x2);
	  std::swap(y1, y2);
	}
	 
	const float dx = x2 - x1;
	const float dy = fabs(y2 - y1);
	 
	float error = dx / 2.0f;
	const int ystep = (y1 < y2) ? 1 : -1;
	int y = (int)y1;

	int maxY = (int)y2;
	 
	const int maxX = (int)x2;
	 
	for(int x=(int)x1; x<maxX; x++)
	{
		if(y < y2)
			{
			std::cout << x << " " << y << std::endl;
		    if(steep)
		    {
		    	(*heightMap)[y][x] = 1;
		    }
		    else
		    {
		        (*heightMap)[x][y] = 1;
		    }
		 
			error -= dy;
			if(error < 0)
			{
		    	y += ystep;
		    	error += dx;
			}
		} else {
			break;
		}
	}

	// //Swap around variables to make startPoint be top-left with slow decline to bottom-right

	// //Checks if the gradient is a steep or shallow gradient
	// bool steep = (std::fabs(endPoint[1] - endPoint[0]) > std::fabs(startPoint[1] - startPoint[0]));

	// if(steep)
	// {
	// 	std::swap(startPoint[0], startPoint[1]);
	// 	std::swap(endPoint[0], endPoint[1]);
	// }

	// //Make sure its going right to left
	// if (startPoint[0] > endPoint[0])
	// {
	// 	std::swap(startPoint[0], endPoint[0]);
	// 	std::swap(startPoint[1], endPoint[1]);
	// }


	// int deltaX = endPoint[0] - startPoint[0];
	// int deltaY = std::fabs(endPoint[1] - startPoint[1]);
	// float error = (float)deltaX / 2.0f;

	// // float deltaError = 0;

	// // if (deltaX == 0)
	// // {
	// // 	deltaError = 1;
	// // } else {
	// // 	deltaError = std::fabs((float)deltaY / (float)deltaX);
	// // }

	// // std::cout << deltaError << std::endl;

	// int yStep = 0;

	// if ( endPoint[1] > startPoint[1])
	// {
	// 	yStep = 1;
	// } else {
	// 	yStep = -1;
	// }

	// int y = startPoint[1];

	// for ( int x = startPoint[0]; x <= endPoint[0]; x++)
	// {
	// 	// std::cout << x << " " << y << " " << error << " " << deltaX << " " << deltaY << std::endl;
	// 	if(steep)
	// 	{
	// 		(*heightMap)[y][x] = 1;
	// 	} else {
	// 		(*heightMap)[x][y] = 1;
	// 	}
		
	// 	error -= deltaY;
	// 	if (error < 0)
	// 	{
	// 		y += yStep;
	// 		error += deltaX;
	// 	}
	// }
}

int insertPointAndFindTriangle(std::vector<std::vector<int> > *coordinates, std::vector<std::vector<int> > triangles)
{
	int x = (rand() % SQUARE_SIZE);
	int y = (rand() % SQUARE_SIZE);

	coordinates->push_back(std::vector<int>{x, y});

	int insideTriangle = -1;

	for (unsigned j = 0; j < triangles.size(); j++)
	{
		std::vector<int> point0 = (*coordinates)[triangles[j][0]];
		std::vector<int> point1 = (*coordinates)[triangles[j][1]];
		std::vector<int> point2 = (*coordinates)[triangles[j][2]];

		if (pointInTriangle(x, y, point0[0], point0[1], point1[0], point1[1], point2[0], point2[1]))
		{
			insideTriangle = j;
			// std::cout << insideTriangle << std::endl;
			break;
		}

	}

	return insideTriangle;

}

float crossProduct(int p1x, int p1y, int p2x, int p2y, int p3x, int p3y)
{
	return ((p1x - p3x) * (p2y - p3y)) - ((p2x - p3x) * (p1y - p3y));
}


//Returns true if point is in triangle
bool pointInTriangle(int pTestx, int pTesty, int p1x, int p1y, int p2x, int p2y, int p3x, int p3y)
{
	bool b1, b2, b3;

	b1 = crossProduct(pTestx, pTesty, p1x, p1y, p2x, p2y) < 0;
	b2 = crossProduct(pTestx, pTesty, p2x, p2y, p3x, p3y) < 0;
	b3 = crossProduct(pTestx, pTesty, p3x, p3y, p1x, p1y) < 0;

	return ((b1 == b2) && (b2 == b3));
}

void connectTriangle(std::vector< std::vector<int> > *triangles, std::vector< std::vector<int> > *coordinates, int insideTriangle)
{
	int lastCoordinateIndex = coordinates->size() - 1;
	std::vector<int> newTriangle1 = {lastCoordinateIndex, (*triangles)[insideTriangle][0], (*triangles)[insideTriangle][1]};
	std::vector<int> newTriangle2 = {lastCoordinateIndex, (*triangles)[insideTriangle][1], (*triangles)[insideTriangle][2]};
	std::vector<int> newTriangle3 = {lastCoordinateIndex, (*triangles)[insideTriangle][2], (*triangles)[insideTriangle][0]};

	(*triangles).push_back(newTriangle1);
	(*triangles).push_back(newTriangle2);
	(*triangles).push_back(newTriangle3);

	(*triangles).erase((*triangles).begin() + insideTriangle);
}

void printToFile(std::vector< std::vector<int> > triangles, std::vector< std::vector<int> > coordinates, std::vector< std::vector<int> > weightMatrix)
{
	std::ofstream outputHeightMap;
	outputHeightMap.open("coordinates.txt");

	int index = 0;

	// std::cout << "Height: " << height << " " << "Width: " << width << " " << "vector Length: " << imageOnlyGrey.size() << std::endl; 

	for ( unsigned i = 0; i < triangles.size(); i++)
	{
		outputHeightMap << i << " : " << triangles[i][0] << " " << triangles[i][1] << " " << triangles[i][2] << " " << std::endl;
	}

	for ( unsigned i = 0; i < coordinates.size(); i++)
	{
		// std::cout << "i: " << i << std::endl;

		outputHeightMap << "( " << coordinates[i][0] << ", " << coordinates[i][1] << " )" << std::endl;
	}

	for ( unsigned i = 0; i < weightMatrix.size(); i++)
	{
		for ( unsigned j = 0; j < weightMatrix.size(); j++)
		{
			outputHeightMap << weightMatrix[i][j] << " ";
		}

		outputHeightMap << std::endl;
	}
}

void findWeightMatrix(std::vector< std::vector<int> > triangles, std::vector< std::vector<int> > coordinates, std::vector< std::vector<int> > *weightMatrix)
{
	//loop trhough triangles
	//weight = distance between coordinates
	//fill weight matrix

	for (unsigned i = 0; i < triangles.size(); i++)
	{
		int *point0 = &triangles[i][0];
		int *point1 = &triangles[i][1];
		int *point2 = &triangles[i][2];

		// std::cout << *point0 << " " << *point1 << " " << *point2 << " " << std::endl;

		//Uses manhattan distance
		float distance01 = (coordinates[*point0][0] - coordinates[*point1][0]) + (coordinates[*point0][0] - coordinates[*point1][1]);
		float distance12 = (coordinates[*point1][0] - coordinates[*point2][0]) + (coordinates[*point1][0] - coordinates[*point2][1]);
		float distance20 = (coordinates[*point2][0] - coordinates[*point0][0]) + (coordinates[*point2][0] - coordinates[*point0][1]);

		//Opposite weights set to 0 as the graph does not flow backwards.
		if (distance01 < 0)
		{
			(*weightMatrix)[*point1][*point0] = 0;
			(*weightMatrix)[*point0][*point1] = -distance01;
		} else {
			(*weightMatrix)[*point1][*point0] = distance01;
			(*weightMatrix)[*point0][*point1] = 0;
		}
		if (distance12 < 0)
		{
			(*weightMatrix)[*point2][*point1] = 0;
			(*weightMatrix)[*point1][*point2] = -distance12;
		} else {
			(*weightMatrix)[*point2][*point1] = distance12;
			(*weightMatrix)[*point1][*point2] = 0;
		}
		if (distance20 < 0)
		{
			(*weightMatrix)[*point0][*point2] = 0;
			(*weightMatrix)[*point2][*point0] = -distance20;
		} else {
			(*weightMatrix)[*point0][*point2] = distance20;
			(*weightMatrix)[*point2][*point0] = 0;
		}

	}
}

std::vector< std::vector<int> > findConnections()
{
	//Connect each point to 2 other random points
	//Then connect a random number of arcs, around numNodes/2
	std::vector< std::vector<int> > connections;
	std::vector<int> numConnections(NUM_NODES, 0);
	for ( int i = 0; i < NUM_NODES; i++)
	{
		std::vector<int> nodeConnections;
		for (int j = 0; j < (2 + (rand()%2)); j++)
		{
			//can be changed to change graphs
			if (numConnections[i] < 2)
			{
				int randConn = i;
				while (i == randConn)
				{
					randConn = (rand() % NUM_NODES);
				}
				nodeConnections.push_back(randConn);

				numConnections[i]++;
			}
		}

		connections.push_back(nodeConnections);

	}

	return connections;
}

std::vector< std::vector< std::vector<int> > >  dijkstra(std::vector< std::vector<int> > connections, std::vector< std::vector<int> > weightMatrix, std::vector<int> usedNodes)
{
	//Make a matrix filled with vectors
	std::vector< std::vector< std::vector<int> > > shortestPaths(connections.size(), std::vector<std::vector<int> > (weightMatrix.size(), std::vector<int> (1, -1)));
	for (unsigned i = 0; i < connections.size(); i++)
	{
		int startNode = usedNodes[i];
		for (unsigned j = 0; j < connections[i].size(); j++)
		{
			int endNode = usedNodes[connections[i][j]];

			// std::cout << startNode << " " << endNode << std::endl;
			int n = weightMatrix.size();
			std::vector<int> shortPath;
			//set distance to a value much higher than will be obtained
			std::vector<int> distance(n, 9999999);
			std::vector<bool> visited(n, false);

			//First node
			distance[startNode] = 0;
			//Init path to start at i node
			shortestPaths[i][startNode] = {startNode};

			for( int x = 0; x < n; x++)
			{
				int currentNode = -1;
				for (int y = 0; y < n; y++)
				{
					//Only check unvisited nodes
					if(!visited[y])
					{
						//Check to see if there is a current node or if the distance to the next node is lower than the distance to the current node
						if(currentNode == -1 || distance[y] < distance[currentNode])
						{
							currentNode = y;
						}
					}
				}

				visited[currentNode] = true;
				for (int y = 0; y < n; y++)
				{
					if (weightMatrix[currentNode][y] != 0)
					{
						int path = distance[currentNode] + weightMatrix[currentNode][y];

						if (path < distance[y])
						{
							distance[y] = path;
							std::vector<int> tempVector = shortestPaths[i][currentNode];
							// std::cout << "Current: " << startNode << " Y: " << y << std::endl;
							tempVector.push_back(y);
							shortestPaths[i][y] = tempVector;
						}
					}
				}
			}
		}
	}

	return shortestPaths;
}