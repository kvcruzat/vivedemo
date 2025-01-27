#include "triangulation.h"


// Uses a 1000 x 1000 grid
//Inserts 100 points into grid
//Picks 4 maxima points/4 minima points / 2 others
//Generates the graph

//Another wya to do this would be to tesselate the swauare, find intersections then use random intersections find 2 connections from each point.

int main (int argc, char *argv[])
{
	srand (time(NULL));

	//Initialisea 100x100 matrix to be all zeros
	//Triangles list keeps track of the points in each triangle
	//Coordinates are stored as indexes to coordinates array
	std::vector<std::vector<int> > triangles;
	//Coordinates are stored in this array as an x point and y point
	std::vector<std::vector<int> > coordinates = findNodes();

	std::vector<std::vector<int> > graphConnections = addLinesToGraph();

	//Add first two triangles to array. Like this
	//0 - 0
	//| \ |
	//0 - 0
	// triangles.push_back(std::vector<int>{0, 1, 3});
	// triangles.push_back(std::vector<int>{0, 2, 3});

	//Initialise matrix for weights of each arc
	std::vector<std::vector<float> > weightMatrix(coordinates.size(), std::vector<float> (NUM_POINTS * NUM_POINTS, 0));

	//Function to find all the weights of connected arcs
	findWeightMatrix(&graphConnections, coordinates, &weightMatrix);
	//Initialise vector to store which nodes are used
	std::vector< int > usedNodes;

	//Use top left and bottom right always.
	usedNodes.push_back(0);
	usedNodes.push_back((NUM_POINTS * NUM_POINTS) - 1);

	

	//For loop to insert which nodes are used into the used nodes array
	for (int i = 0; i < NUM_NODES - 2; i++)
	{
		//Get a random node
		//Exluding guaranteed nodes
		int randNode = (rand() % (NUM_POINTS * NUM_POINTS));
		//While the node doesn't exist in the usedNodes array
		while(std::find(usedNodes.begin(), usedNodes.end(), randNode) != usedNodes.end()) {
			//Find a new random node
    		randNode = (rand() % (NUM_POINTS * NUM_POINTS));
    	}
    	//Add the node if it doesn't exist
    	usedNodes.push_back(randNode);
    	// std::cout << randNode << std::endl;
	}

	//Initialise array to be used to keep track of connections in graph
	std::vector< std::vector<int> > connections;
	//Picks random points to use as nodes
	findConnections(&connections, usedNodes, coordinates);

	//TODO: CARRY ON HERE TOMORROW

	//Use djikstra to find the shortest paths between each node
	std::vector< std::vector< std::vector<int> > > shortestPaths = dijkstra(connections, weightMatrix, usedNodes);

	//Change coordinates to match the grid size
	updateCoordinates(&coordinates);

	//Output all information
	// printToFile(triangles, coordinates, weightMatrix);
	// testPrint(shortestPaths, usedNodes, connections);
	printGraph(shortestPaths, coordinates, connections, usedNodes);
}


/* Function to print debugging info
 * Input - 
 * shortestPath - vector that contains the shortest paths from the used nodes to every other node
 * usedNodes - vector that contains the indices of the nodes used in the main graph
 * connections - matrix that shows which nodes are connected to the other nodes
 */
void testPrint(std::vector< std::vector< std::vector<int> > > shortestPaths, std::vector< int > usedNodes, std::vector< std::vector<int> > connections)
{
	//Open an output file stream and direct it to the correct file
	std::ofstream outputHeightMap;
	outputHeightMap.open("paths.txt");

	//Loop through every used node and all its possible connections
	for ( unsigned i = 0; i < shortestPaths.size(); i++)
	{
		for ( unsigned j = 0; j < shortestPaths[i].size(); j++)
		{
			//Output the start node and which node it'll go to
			outputHeightMap << usedNodes[i] << " to " << j << " : ";
			//Loop through the path
			for (unsigned k = 0; k < shortestPaths[i][j].size(); k++)
			{
				//Print the path
				outputHeightMap << shortestPaths[i][j][k] << " ";
			}

			outputHeightMap << std::endl << std::endl;
		}
	}


	//For loop to print which nodes are connected
	for ( unsigned i = 0; i < connections.size(); i++)
	{
		for ( unsigned j = 0; j < connections[i].size(); j++)
		{
			outputHeightMap << usedNodes[i] << " to " << usedNodes[connections[i][j]] << " " << std::endl;
		}

		outputHeightMap << std::endl << std::endl;
	}
}

/* Function that prints the connection matrix and the coordinates of each point
 * Also changes the connection matrix from the main points to a connection matrix that includes the middle points
 * Input -
 * shortestPath - vector that contains the shortest paths from the used nodes to every other node
 * coordinates - a vector that stores every nodes coordinates
 * connections - matrix that shows which nodes are connected to the other nodes
 * usedNodes - vector that contains the indices of the nodes used in the main graph
 */
void printGraph(std::vector< std::vector< std::vector<int> > > shortestPaths, std::vector<std::vector<int> > coordinates, std::vector< std::vector<int> > connections, std::vector< int > usedNodes)
{
	//Open output file stream and direct it to correct file
	std::ofstream outputGraph;
	outputGraph.open("graphs.txt");

	//Initialise a matrix to be used to store all connections
	std::vector<std::vector<int> > connectionsMatrix( shortestPaths[0].size(), std::vector<int> (shortestPaths[0].size(), 0));
	//Loop through each node in the input connections
	for (unsigned i = 0; i < connections.size(); i++)
	{
		for (unsigned j = 0; j < connections[i].size(); j++)
		{
			//Create a pointer so that it is easier to read
			std::vector<int> *ptrShortPath = &shortestPaths[i][usedNodes[connections[i][j]]];
			//Loop through each shortest path
			for(unsigned k = 0; k < shortestPaths[i][usedNodes[connections[i][j]]].size() - 1; k++)
			{
				//If the two nodes are connected then set the connection matrix to show that they are
				if ((*ptrShortPath)[k] != -1)
				{
					connectionsMatrix[(*ptrShortPath)[k]][(*ptrShortPath)[k+1]] = 1;
				}
			}
		}
	}

	//For loop to print the connection matrix
	for (unsigned i = 0; i < connectionsMatrix.size(); i++)
	{
		for (unsigned j = 0; j < connectionsMatrix[i].size(); j++)
		{
			outputGraph << connectionsMatrix[i][j] << " ";
		}

		outputGraph << std::endl;
	}
	outputGraph << std::endl << std::endl;

	//For loop to print the coordinates
	for (unsigned i = 0; i < coordinates.size(); i++)
	{
		outputGraph << coordinates[i][0] << ", " << coordinates[i][1] << std::endl;
	}

	//Print the height map
	printHeightMap(connectionsMatrix, coordinates);
}

/* Function that outputs the heightmap of the graph to a file
 * Input -
 * connectionsMatrix -  Matrix that contains the information about which nodes are connected
 * coordinates - a vector that stores every nodes coordinates
 */
void printHeightMap(std::vector< std::vector<int> > connectionsMatrix, std::vector< std::vector<int> > coordinates)
{
	//Canal height map
	//If 1 then canal is present
	std::vector< std::vector<float> > heightMap(SQUARE_SIZE, std::vector<float> (SQUARE_SIZE, 1));

	//Loop through the connections matrix
	for (unsigned i = 0; i < connectionsMatrix.size(); i++)
	{
		for (unsigned j = 0; j < connectionsMatrix[i].size(); j++)
		{
			//Check if two nodes are connected
			//If yes then use Bresenham's Line algorithm to find
			//The "river" connecting them
			if ( connectionsMatrix[i][j] != 0)
			{
				//Bresenham's Line algorithm
				drawLine(&heightMap, coordinates[i][0], coordinates[j][0], coordinates[i][1], coordinates[j][1]);
			}
		}
	}

	//Change the heightmap so that the "rivers" are not 1 pixel wide
	makeDitches(&heightMap);

	//Open an output file stream and point it to the height map file
	std::ofstream graphHeightMap;
	graphHeightMap.open("heightmaps/graphHeightMap.txt");

	//Print the heightmap to this file
	for (unsigned i = 0; i < heightMap.size(); i++)
	{
		for (unsigned j = 0; j < heightMap[i].size(); j++)
		{
			graphHeightMap << heightMap[i][j] << " ";
		}

		graphHeightMap << std::endl;
	}
}

/* Functions that changes the width of the "rivers" in the heightmap
 * Rivers are made using the graph for x^4
 * Input -
 * *oldHeightMap - pointer that points to the heightmap before any changes have been made to it
 */
void makeDitches(std::vector< std::vector<float> > *oldHeightMap)
{
	//Variables used to make ditches
	//Increase kmax for wider rivers
	int kmax = 50;
	int interval = kmax/3;
	float ditchHeight = 0.33f;
	//Increase ksquared with kmax
	int kMaxSquared = kmax * kmax * kmax * kmax;
	//Initialise the new height map to be the same as the old one.
	std::vector< std::vector<float> > heightMap = (*oldHeightMap);
	//Loop through the heightmap
	for (int i = 0; i < (heightMap).size(); i++)
	{
		for (int j = 0; j < (heightMap)[i].size(); j++)
		{
			//If the old heightmap has a ditch
			if ((*oldHeightMap)[i][j] == 0.0f)
			{
				//Loop through k times and change height on either side of "river"
				//Could use exponent? smoother curves
				for (int k = 0; k < kmax; k++)
				{
					//Check so the coordinate is not out of bounds
					if (j + k < SQUARE_SIZE)
					{
						//Adjust height
						// if (heightMap[i][j + k] > (((float)k * (float)k * (float)k * (float)k) / (float)kMaxSquared));
						// {
						// 	heightMap[i][j + k] = (((float)k * (float)k * (float)k * (float)k) / (float)kMaxSquared);
						// }
						if ( k < interval)
						{
							if (heightMap[i][j+k] > 0)
							{
								heightMap[i][j+k] = 0;
							}
						} else if ( k < 2*interval)
						{
							if (heightMap[i][j+k] > ditchHeight)
							{
								heightMap[i][j+k] = ditchHeight;
							}
						} else {
							if (heightMap[i][j+k] > 2*ditchHeight)
							{
								heightMap[i][j+k] = 2*ditchHeight;
							}
						}
					}
					//Check so the coordinate is not out of bounds
					if (j - k > 0)
					{
						if ( k < interval)
						{
							if (heightMap[i][j-k] > 0)
							{
								heightMap[i][j-k] = 0;
							}
						} else if ( k < 2*interval)
						{
							if (heightMap[i][j-k] > ditchHeight)
							{
								heightMap[i][j-k] = ditchHeight;
							}
						} else {
							if (heightMap[i][j-k] > 2*ditchHeight)
							{
								heightMap[i][j-k] = 2*ditchHeight;
							}
						}
						//Check so the coordinate is not out of bounds
						// if (heightMap[i][j - k] > (((float)k * (float)k * (float)k * (float)k) / (float)kMaxSquared))
						// {
						// 	heightMap[i][j - k] = (((float)k * (float)k * (float)k * (float)k) / (float)kMaxSquared);
						// }
					}
					//Check so the coordinate is not out of bounds
					if (i + k < SQUARE_SIZE)
					{
						if ( k < interval)
						{
							if (heightMap[i+k][j] > 0)
							{
								heightMap[i+k][j] = 0;
							}
						} else if ( k < 2*interval)
						{
							if (heightMap[i+k][j] > ditchHeight)
							{
								heightMap[i+k][j] = ditchHeight;
							}
						} else {
							if (heightMap[i+k][j] > 2*ditchHeight)
							{
								heightMap[i+k][j] = 2*ditchHeight;
							}
						}
						//Adjust height
						// if (heightMap[i + k][j] > (((float)k * (float)k * (float)k * (float)k) / (float)kMaxSquared));
						// {
						// 	heightMap[i + k][j] = (((float)k * (float)k * (float)k * (float)k) / (float)kMaxSquared);
						// }
					}
					//Check so the coordinate is not out of bounds
					if (i - k > 0)
					{
						if ( k < interval)
						{
							if (heightMap[i-k][j] > 0)
							{
								heightMap[i-k][j] = 0;
							}
						} else if ( k < 2*interval)
						{
							if (heightMap[i-k][j] > ditchHeight)
							{
								heightMap[i-k][j] = ditchHeight;
							}
						} else {
							if (heightMap[i-k][j] > 2*ditchHeight)
							{
								heightMap[i-k][j] = 2*ditchHeight;
							}
						}
						// //Check so the coordinate is not out of bounds
						// if (heightMap[i - k][j] > (((float)k * (float)k * (float)k * (float)k) / (float)kMaxSquared))
						// {
						// 	heightMap[i - k][j] = (((float)k * (float)k * (float)k * (float)k) / (float)kMaxSquared);
						// }
					}
				}
			}
		}
	}

	//Change the old heightmap to the new one
	(*oldHeightMap) = heightMap;
}

/* Function that uses Bresenham's line algorithm to find the line 
 * Input -
 * *heightMap - pointer to the graph heightmap
 * x1 - x coordinate of first point
 * x2 - x coordinate of second point
 * y1 - y coordinate of first point
 * y2 - y coordinate of second point
 */
void drawLine(std::vector< std::vector<float> > *heightMap, int x1, int x2, int y1, int y2)
{
	// Bresenham's line algorithm only works in one octant of a graph
	//Change variables to match this

	//Check the gradient of the slope
	const bool steep = (fabs(y2 - y1) > fabs(x2 - x1));
	//Swap x and y coordinates if gradient is steep
	if(steep)
	{
	  std::swap(x1, y1);
	  std::swap(x2, y2);
	}
	
	//Swap points if line is sloping to left instead of right
	if(x1 > x2)
	{
	  std::swap(x1, x2);
	  std::swap(y1, y2);
	}
	
	//Get the values of the distance between the points 
	const float dx = x2 - x1;
	const float dy = fabs(y2 - y1);
	 
	float error = dx / 2.0f;
	//Check if the y step is up or down
	const int ystep = (y1 < y2) ? 1 : -1;
	int y = (int)y1;

	//Set max y so it doesn't go out of bounds
	int maxY = (int)SQUARE_SIZE;
	//Set max x so it doesn't go past the last x
	const int maxX = (int)x2;
	 
	//Loop over x
	for(int x=(int)x1; x<maxX; x++)
	{
		//Check so y isn't out of bounds
		if(y < maxY)
		{
		    if(steep)
		    {
		    	(*heightMap)[y][x] = 0;
		    }
		    else
		    {
		        (*heightMap)[x][y] = 0;
		    }
		 
			error -= dy;
			//Correct for error
			if(error < 0)
			{
		    	y += ystep;
		    	error += dx;
			}
		} else {
			break;
		}
	}
}

/* Function to generate a random point and find what triangle that point is in
 * Input - 
 * *coordinates - pointer to the coordinates array
 * triangles - vector that stores the triangles	
 */
int insertPointAndFindTriangle(std::vector<std::vector<int> > *coordinates, std::vector<std::vector<int> > triangles)
{
	//Generate two points
	int x = (rand() % SQUARE_SIZE);
	int y = (rand() % SQUARE_SIZE);
	//Add them to coordinates array
	coordinates->push_back(std::vector<int>{x, y});
	//Variable to keep track of which triangle it is in
	int insideTriangle = -1;
	//Loop through each triangle
	for (unsigned j = 0; j < triangles.size(); j++)
	{
		//Get coordinates of each corner of triangle
		std::vector<int> point0 = (*coordinates)[triangles[j][0]];
		std::vector<int> point1 = (*coordinates)[triangles[j][1]];
		std::vector<int> point2 = (*coordinates)[triangles[j][2]];

		//Check if the point is inside the triangle
		if (pointInTriangle(x, y, point0[0], point0[1], point1[0], point1[1], point2[0], point2[1]))
		{
			//Set variable to the triangle point is in
			insideTriangle = j;
			//Break loop
			break;
		}
	}

	return insideTriangle;
}

/* Function that gets the cross product of the three points
 * Input -
 * coordinates of each point
 * Output
 * float - cross product
 */
float crossProduct(int p1x, int p1y, int p2x, int p2y, int p3x, int p3y)
{
	return ((p1x - p3x) * (p2y - p3y)) - ((p2x - p3x) * (p1y - p3y));
}


/* Function that determines if a point is in the triangle
 * Input -
 * test point coordinates
 * coordinates of triangle points
 * Output - 
 * bool - whether point is inside triangle
 */
bool pointInTriangle(int pTestx, int pTesty, int p1x, int p1y, int p2x, int p2y, int p3x, int p3y)
{
	bool b1, b2, b3;

	b1 = crossProduct(pTestx, pTesty, p1x, p1y, p2x, p2y) < 0;
	b2 = crossProduct(pTestx, pTesty, p2x, p2y, p3x, p3y) < 0;
	b3 = crossProduct(pTestx, pTesty, p3x, p3y, p1x, p1y) < 0;

	return ((b1 == b2) && (b2 == b3));
}

/* Function that deletes the old triangle and connects the new one
 * Input -
 * *triangles - Pointer to array of indices of each point of the triangle
 * *coordinates - Pointer to coordinate array
 * insideTriangle - int that points to index of triangle that point is inside
 */
void connectTriangle(std::vector< std::vector<int> > *triangles, std::vector< std::vector<int> > *coordinates, int insideTriangle)
{
	//Get the last index in the coordinate array
	int lastCoordinateIndex = coordinates->size() - 1;
	//Create the three new triangles with the coordinates of the newest point and the three other points of the triangle
	std::vector<int> newTriangle1 = {lastCoordinateIndex, (*triangles)[insideTriangle][0], (*triangles)[insideTriangle][1]};
	std::vector<int> newTriangle2 = {lastCoordinateIndex, (*triangles)[insideTriangle][1], (*triangles)[insideTriangle][2]};
	std::vector<int> newTriangle3 = {lastCoordinateIndex, (*triangles)[insideTriangle][2], (*triangles)[insideTriangle][0]};

	//Add new triangles to the triangle array
	(*triangles).push_back(newTriangle1);
	(*triangles).push_back(newTriangle2);
	(*triangles).push_back(newTriangle3);

	//Delete the triangle that the point was inside
	(*triangles).erase((*triangles).begin() + insideTriangle);
}

/* Function that prints which coordinates are the corners of each triangle and also prints the weight matrix
 * Input - 
 * triangles - array containing which coordinates are the corners of each triangle
 * coordinates - array containing the coordinates of each point
 * weightMatrix - a matrix containing all the weights of each arc
 */
void printToFile(std::vector< std::vector<int> > triangles, std::vector< std::vector<int> > coordinates, std::vector< std::vector<float> > weightMatrix)
{
	//Open an output stream pointing to the right file
	std::ofstream outputHeightMap;
	outputHeightMap.open("coordinates.txt");
	//int index = 0;

	//Loop through each triangle
	for ( unsigned i = 0; i < triangles.size(); i++)
	{
		outputHeightMap << i << " : " << triangles[i][0] << " " << triangles[i][1] << " " << triangles[i][2] << " " << std::endl;
	}

	//Print each coordinate
	for ( unsigned i = 0; i < coordinates.size(); i++)
	{
		outputHeightMap << "( " << coordinates[i][0] << ", " << coordinates[i][1] << " )" << std::endl;
	}

	//Print the weight matrix
	for ( unsigned i = 0; i < weightMatrix.size(); i++)
	{
		for ( unsigned j = 0; j < weightMatrix.size(); j++)
		{
			outputHeightMap << weightMatrix[i][j] << " ";
		}

		outputHeightMap << std::endl << std::endl;
	}
}

/* Function that finds the weights between each connected point
 * Input -
 * triangles - array containing which coordinates are the corners of each triangle
 * coordinates - array containing the coordinates of each point
 * weightMatrix - pointer to a matrix containing all the weights of each arc
 */
void findWeightMatrix(std::vector< std::vector<int> > *graphConnections, std::vector< std::vector<int> > coordinates, std::vector< std::vector<float> > *weightMatrix)
{
	//loop trhough triangles
	//weight = distance between coordinates
	//fill weight matrix

	//Loop through each triangle
	for (unsigned i = 0; i < (*graphConnections).size(); i++)
	{
		for (unsigned j = 0; j < (*graphConnections)[i].size(); j++)
		{
			int connectionPoint = (*graphConnections)[i][j];

			// std::cout << (*connections)[99][0] << " " << (i * NUM_POINTS) + j << std::endl;
			// std::cout << i << " connected to: " << (*connections)[99][0] << std::endl;
			//Find manhattan distance between the points

			float xWeighted = 1f;
			float yWeighted = 1f;

			int xSquared = (coordinates[i][0] - coordinates[connectionPoint][0]) * (coordinates[i][0] - coordinates[connectionPoint][0]);
			int ySquared = (coordinates[i][1] - coordinates[connectionPoint][1]) * (coordinates[i][1] - coordinates[connectionPoint][1]);

			xWeighted = xWeighted * xSquared;
			yWeighted = yWeighted * ySquared;

			float distance = std::sqrt(float(xWeighted) + float(yWeighted));

			float manhattanDist = (coordinates[i][0] - coordinates[connectionPoint][0]) + (coordinates[i][1] - coordinates[connectionPoint][1]);

			//Set the weights in the matrix
			//Opposite weights set to 0 as the graph does not flow backwards.
			if (manhattanDist < 0)
			{
				(*weightMatrix)[connectionPoint][i] = 0;
				(*weightMatrix)[i][connectionPoint] = -distance;
			} else {
				(*weightMatrix)[connectionPoint][i] = distance;
				(*weightMatrix)[i][connectionPoint] = 0;
			}
		}

	}
}

/* Function to determine which nodes are connected
 * Input - *connections - pointer to an array that connections whihc main nodes are connected
 */
void findConnections(std::vector<std::vector<int> > *connections, std::vector<int> usedNodes, std::vector<std::vector<int> > coordinates)
{
	//Connect each point to 2 other random points
	//Then connect a random number of arcs, around numNodes/2
	std::vector<int> numConnections(NUM_NODES, 0);

	std::vector<int> distances;
	std::vector<int> distancesUnsorted;
	std::vector<int> order;

	//Don't let it connect to nodes on the edge
	std::vector<int> disallowedNodes;

	for (int i  = 0; i < NUM_POINTS; i++)
	{
		for (int j = 0; j < NUM_POINTS; j++)
		{
			if ( j == 0 || i == 0)
			{
				int currentPoint = (i * NUM_POINTS) + j;
				if (!(currentPoint == 0 || currentPoint == (NUM_POINTS * NUM_POINTS) - 1))
				{
					disallowedNodes.push_back(currentPoint);
				}
			}
		}
	}

	for (unsigned i = 0; i < usedNodes.size(); i++)
	{
		distances.push_back(coordinates[usedNodes[i]][0] + coordinates[usedNodes[i]][1]);
		distancesUnsorted.push_back(coordinates[usedNodes[i]][0] + coordinates[usedNodes[i]][1]);
		// std::cout << usedNodes[i] << std::endl;
	}

	std::sort(distances.begin(), distances.end());

	for (unsigned i  = 0; i < usedNodes.size(); i++)
	{
		for (unsigned j = 0; j < usedNodes.size(); j++)
		{
			if (distances[i] == distancesUnsorted[j] && std::find(order.begin(), order.end(),j)==order.end())
			{
				order.push_back(j);
				std::cout << distances[i] << " " << j << std::endl;
				break;
			}
		}
	}

	//Loop through the amount of nodes you want to add
	for ( int i = 0; i < NUM_NODES; i++)
	{
		//Vector to store connections
		std::vector<int> nodeConnections;
		//Loop through a random amount of times, either 2, 3, or 4
		for (int j = 0; j < 2; j++)
		{
			//can be changed to change graphs
			//Check so node only has 2 connections
			if (numConnections[i] < 2)
			{
				// //Find a connection that isn't already used
				// int randConn = i;
				// while (i == randConn || std::find(nodeConnections.begin(), nodeConnections.end(),randConn)!=nodeConnections.end())
				// {
				// 	randConn = (rand() % NUM_NODES);
				// }

				for(unsigned k = 0; k < usedNodes.size(); k++)
				{
					if (i != order[k] && std::find(nodeConnections.begin(), nodeConnections.end(),order[k])==nodeConnections.end() && std::find(disallowedNodes.begin(), disallowedNodes.end(),order[k])==disallowedNodes.end())
					{
						if (distances[k] > distancesUnsorted[i])
						{
							std::cout << distances[k] << " > " << distancesUnsorted[i] << " " << order[k] << " > " << i << std::endl;;
							nodeConnections.push_back(order[k]);
							// numConnections[order[k]]++;
							break;
						}
					}
				}

				//ADd it to the vector
				// nodeConnections.push_back(randConn);

				numConnections[i]++;
				// numConnections[randConn]++;
			}
		}
		//ADd the new connections to the total vector
		(*connections).push_back(nodeConnections);

	}

	for (int i = 0; i < (*connections).size(); i++)
	{
		std::cout << i << " to: ";

		for (int j = 0; j < (*connections)[i].size(); j++)
		{
			std::cout << (*connections)[i][j] << " ";
		}

		std::cout << std::endl;
	}
}

/* Function to find all the shortest paths from one node to the other nodes
 * Input -
 * connections - Vector that contains which nodes are connected
 * weightMatrix - matrix that contains the weights between all the nodes
 * usedNodes - vector that contains the nodes that have been used
 * Output -
 * shortest path from the nodes to other nodes
 */
std::vector< std::vector< std::vector<int> > >  dijkstra(std::vector< std::vector<int> > connections, std::vector< std::vector<float> > weightMatrix, std::vector<int> usedNodes)
{
	//Make a matrix filled with vectors
	std::vector< std::vector< std::vector<int> > > shortestPaths(connections.size(), std::vector<std::vector<int> > (weightMatrix.size(), std::vector<int> (1, -1)));
	//loop through the entire connections matrix
	for (unsigned i = 0; i < connections.size(); i++)
	{
		for (unsigned j = 0; j < connections[i].size(); j++)
		{
			//Find shortest path from i to j
			int startNode = usedNodes[i];
			int endNode = usedNodes[connections[i][j]];

			//initialise variables
			int n = weightMatrix.size();
			std::vector<int> shortPath;
			//set distance to a value much higher than will be obtained
			std::vector<int> distance(n, 99999999);
			std::vector<bool> visited(n, false);

			//First node
			//Distance to first node is always 0 from itself
			distance[startNode] = 0;
			//Init path to start at i node
			shortestPaths[i][startNode] = {startNode};

			//Loop through each node
			for( int x = 0; x < n; x++)
			{
				int currentNode = -1;
				//Loop through each node to find closest node
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

				//set currentNode to be visited
				visited[currentNode] = true;
				//Loop through each node
				for (int y = 0; y < n; y++)
				{
					//Check if there is a connection between the nodes
					if (weightMatrix[currentNode][y] != 0)
					{
						//New distance is old distance + connection distance
						int path = distance[currentNode] + weightMatrix[currentNode][y];
						//Check to see if the new path is shorter than the one already found
						if (path < distance[y])
						{
							//Set distance to the new distance
							distance[y] = path;
							//Vector to hold the shortest path
							std::vector<int> tempVector = shortestPaths[i][currentNode];
							tempVector.push_back(y);
							//Set the shortest path to the path found
							shortestPaths[i][y] = tempVector;
							//Check to see if the backwards path has a path found already
							std::vector<int>::iterator iter = std::find(usedNodes.begin(), usedNodes.end(),y);
							if(iter != usedNodes.end())
							{
								//If no path found then set path to the one found
								int index = std::distance(usedNodes.begin(), iter);
								// std::cout << y << std::endl;
								shortestPaths[index][usedNodes[i]] = tempVector;
							}
						}
					}
				}
			}
		}
	}

	return shortestPaths;
}

std::vector< std::vector<int> > addLinesToGraph()
{
	std::vector < std::vector <int> > connections;
	for (int i = 0; i < NUM_POINTS; i++)
	{
		for (int j = 0; j < NUM_POINTS; j++)
		{
			std::vector<int> connectedness;
			int currentPoint = (i * NUM_POINTS) + j;
			//Find 8 connectedness
			//Left
			if (!(j - 1 < 0))
			{
				connectedness.push_back(currentPoint - 1);
			}
			//Right
			if (!(j + 1 >= NUM_POINTS))
			{
				connectedness.push_back(currentPoint + 1);
			}
			//Up
			if (!(i - 1 < 0))
			{
				connectedness.push_back(currentPoint - (NUM_POINTS));
			}
			//Down
			if (!(i + 1 >= NUM_POINTS))
			{
				connectedness.push_back(currentPoint + (NUM_POINTS));
			}
			//Up-Right
			if (!(i - 1 < 0 || j + 1 >= NUM_POINTS))
			{
				connectedness.push_back(currentPoint + 1 - (NUM_POINTS));
			}
			//Down-Right
			if (!(i + 1 >= NUM_POINTS || j + 1 >= NUM_POINTS))
			{
				connectedness.push_back(currentPoint + 1 + (NUM_POINTS));
			}
			//Down-Left
			if (!(i + 1 >= NUM_POINTS || j - 1 < 0))
			{
				connectedness.push_back(currentPoint - 1 + (NUM_POINTS));
			}
			//Up-Left
			if (!(i - 1 < 0 || j - 1 < 0))
			{
				connectedness.push_back(currentPoint - 1 - (NUM_POINTS));
			}

			connections.push_back(connectedness);
		}
	}

	return connections;
}

std::vector<std::vector<int> > findNodes()
{
	std::vector< std::vector<int> > coordinates;
	for (int i = 0; i < (NUM_POINTS); i++)
	{
		for (int j = 0; j < (NUM_POINTS); j++)
		{
			coordinates.push_back(std::vector<int>{i, j});
		}
	}

	return coordinates;
}

void updateCoordinates(std::vector< std::vector<int> > *coordinates)
{
	//int to hold the scale factor
	int scale = SQUARE_SIZE / NUM_POINTS;

	for (int i = 0; i < coordinates->size() - 1; i++)
	{
		(*coordinates)[i][0] = (*coordinates)[i][0] * scale;
		(*coordinates)[i][1] = (*coordinates)[i][1] * scale;
	}

	(*coordinates)[(coordinates->size()) - 1][0] = SQUARE_SIZE;
	(*coordinates)[(coordinates->size()) - 1][1] = SQUARE_SIZE;
}