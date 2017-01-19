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

	//Use djikstra to find the shortest paths between each node
	std::vector< std::vector< std::vector<int> > > shortestPaths = dijkstra(connections, weightMatrix, usedNodes);

	//Change coordinates to match the grid size
	updateCoordinates(&coordinates);

	//Output all information
	// printToFile(triangles, coordinates, weightMatrix);
	// connectionsPrint(shortestPaths, usedNodes, connections);
	findNewNodesAndRemoveEdges(shortestPaths, coordinates, connections, usedNodes, &weightMatrix);

	// outputNodes(usedNodes, coordinates);
}


/* Function to output connection info
 */
void connectionsPrint(std::vector< std::vector< std::vector<int> > > shortestPaths, std::vector< int > usedNodes, std::vector< std::vector<std::string> > rodIndex, std::vector< std::string> riverNames)
{
	//Open output streams and connect them to correct files
	std::ofstream connectionsFile;
	connectionsFile.open("../../RiversofHanoi/Content/models/connections.txt");

	std::ofstream nodeConnectionsFile;
	nodeConnectionsFile.open("../../RiversofHanoi/Content/models/nodeConnections.txt");

	//new strategy, loop trhough rodIndex array. Connection starts with rodIndex value. From second point find shortestPath to each other node, including bottom-left
	//Then find shortest path of the paths found

	// std::cout << usedNodes.size() << std::endl;

	//Create a new 2D vector to store connections bewteen nodes in
	std::vector<std::vector<int> > nodeConnections(usedNodes.size() + 1, std::vector<int>(0, 0));

	//Create variables
	int rodIndexSize;

	std::vector<std::string> startNodes;
	std::vector<std::string> endNodes;

	std::string point0River;

	//Gets the length of the name of a node
	int riverNameSize = std::to_string(NUM_POINTS).length();

	//Loop through the rivers
	for (unsigned i = 0; i < riverNames.size(); i++)
	{
		//Add the start node and the end node to correct array
		startNodes.push_back(riverNames[i].substr(0, riverNameSize));
		endNodes.push_back(riverNames[i].substr(riverNameSize, riverNameSize));
	}

	//Variable to store whether point 0 is a node or not
	bool point0Node = false;

	//Loop through the rod index array
	for (unsigned i = 0; i < rodIndex.size(); i++)
	{
		for(unsigned j = 0; j < rodIndex[i].size(); j++)
		{
			//If it finds a rod on point 0 then it is a node
			if (rodIndex[i][j].substr(0, riverNameSize) == "00")
			{
				point0Node = true;
			}	
		}
	}

	//If point 0 is not a node
	if(!point0Node)
	{
		//Loop through the riverNames array
		for(unsigned i = 0; i < riverNames.size(); i++)
		{
			//For the river that starts at node 0
			if (riverNames[i].substr(0,riverNameSize) == "00")
			{
				//Add the point to the start array and the end point to the end array
				startNodes.push_back("00");
				endNodes.push_back(riverNames[i].substr(riverNameSize,riverNameSize));
				//Store which river is the first river
				point0River = riverNames[i];
			}
		}
	}

	//Create a 2D array to store the shortest paths
	std::vector<std::vector<std::string> > newShortestPaths;

	//Loop through the rods to find all the rivers that come from a node
	for(unsigned i = 0; i < rodIndex.size(); i++)
	{
		for(unsigned j = 0; j < rodIndex[i].size(); j++)
		{
			//Find the shortest path between the node and the next node in the graph
			newShortestPaths.push_back(greedyShortestPath(rodIndex[i][j], usedNodes, riverNames, startNodes, endNodes));
		}
	}

	//If point 0 is not a node, also find the shortest path between point 0 and the next node
	if(!point0Node)
	{
		newShortestPaths.push_back(greedyShortestPath(point0River, usedNodes, riverNames, startNodes, endNodes));
	}


	//This is to find the diretions that the rivers are coming into and out of the node from.
	//Loop over the paths
	for (int i = 0; i < newShortestPaths.size(); i++)
	{
		//Initialise variables that will be needed
		//The first node in the connection
		int startNode = std::stoi(newShortestPaths[i][0].substr(0,riverNameSize));
		//The 2nd point in the connection
		int startConnect = std::stoi(newShortestPaths[i][0].substr(riverNameSize,riverNameSize));
		//The end node in the connection
		int endNode = std::stoi(newShortestPaths[i][newShortestPaths[i].size() - 1].substr(riverNameSize,riverNameSize));
		//The 2nd to last node in the connection
		int endConnect = std::stoi(newShortestPaths[i][newShortestPaths[i].size() - 1].substr(0,riverNameSize));
		//The index of the start node in the usedNodes array
		int startIndex = std::find(usedNodes.begin(), usedNodes.end(), startNode) - usedNodes.begin() + 1;
		//The index of the start node in the usedNodes array
		int endIndex = std::find(usedNodes.begin(), usedNodes.end(), endNode) - usedNodes.begin() + 1;

		//Check if the start or end node is the last point in the graph (bottom-right)
		if (startNode == ((NUM_POINTS * NUM_POINTS) - 1))
			startIndex = 0;
		if (endNode == ((NUM_POINTS * NUM_POINTS) - 1))
			endIndex = 0;

		//Check so the start index is a node
		if (startIndex <= usedNodes.size())
		{
			//Set of if statements to check where the river is coming from
			//It doesn this by checking the name of the other point in the river, and comparing to the node name
			//The node direction can be found by calculating the difference in these two numbers
			//North
			if(startNode - NUM_POINTS == startConnect) 
			{
				nodeConnections[startIndex].push_back(0);
			}
			//North-east
			else if(startNode - (NUM_POINTS - 1) == startConnect)
			{
				nodeConnections[startIndex].push_back(1);
			}
			//East
			else if(startNode + 1 == startConnect)
			{
				nodeConnections[startIndex].push_back(2);
			}
			//South-East
			else if(startNode + (NUM_POINTS + 1) == startConnect)
			{
				nodeConnections[startIndex].push_back(3);
			}
			//South
			else if(startNode + NUM_POINTS == startConnect)
			{
				nodeConnections[startIndex].push_back(4);
			}
			//South-West
			else if(startNode + (NUM_POINTS - 1) == startConnect)
			{
				nodeConnections[startIndex].push_back(5);
			}
			//West
			else if(startNode - 1 == startConnect)
			{
				nodeConnections[startIndex].push_back(6);
			}
			//North-West
			else if(startNode - (NUM_POINTS + 1) == startConnect)
			{
				nodeConnections[startIndex].push_back(7);
			}
		}
		//Check so the end index is a node
		if (endIndex < usedNodes.size())
		{
			//Set of if statements to check where the river is coming from
			//It doesn this by checking the name of the other point in the river, and comparing to the node name
			//The node direction can be found by calculating the difference in these two numbers
			//North
			//North
			if(endNode - NUM_POINTS == endConnect) 
			{
				nodeConnections[endIndex].push_back(0);
			}
			//North-east
			else if(endNode - (NUM_POINTS - 1) == endConnect)
			{
				nodeConnections[endIndex].push_back(1);
			}
			//East
			else if(endNode + 1 == endConnect)
			{
				nodeConnections[endIndex].push_back(2);
			}
			//South-East
			else if(endNode + (NUM_POINTS + 1) == endConnect)
			{
				nodeConnections[endIndex].push_back(3);
			}
			//South
			else if(endNode + NUM_POINTS == endConnect)
			{
				nodeConnections[endIndex].push_back(4);
			}
			//South-West
			else if(endNode + (NUM_POINTS - 1) == endConnect)
			{
				nodeConnections[endIndex].push_back(5);
			}
			//West
			else if(endNode - 1 == endConnect)
			{
				nodeConnections[endIndex].push_back(6);
			}
			//North-West
			else if(endNode - (NUM_POINTS + 1) == endConnect)
			{
				nodeConnections[endIndex].push_back(7);
			}
		}
	}

	//Print out the connections to a file
	for (unsigned i = 0; i < newShortestPaths.size(); i++)
	{
		for (unsigned j = 0; j < newShortestPaths[i].size(); j++)
		{
			connectionsFile << newShortestPaths[i][j] << " ";
		}
		connectionsFile << std::endl;
	}

	//need to add two new lines at beginning for formatting
	for(unsigned i = 0; i < 1; i++)
	{
		nodeConnectionsFile << std::endl;
	}

	//Output the node connections to the file
	for (unsigned i = 0; i < nodeConnections.size(); i++)
	{
		std::sort(nodeConnections[i].begin(), nodeConnections[i].end());
		for (unsigned j = 0; j < nodeConnections[i].size(); j++)
		{
			nodeConnectionsFile << nodeConnections[i][j] << " ";
		}

		nodeConnectionsFile << std::endl;
	}
		

	//LEGACY CODE

	// for (unsigned i  = 0; i < rodIndex.size(); i++)
	// {
	// 	for (unsigned j = 0; j < rodIndex[i].size(); j++)
	// 	{
	// 		rodIndexSize++;
	// 	}
	// }

	// int currentIndex = 0;

	// std::vector< std::vector< std::vector < std::string > > > paths(rodIndexSize, std::vector<std::vector<std::string> >(usedNodes.size() + 1, std::vector<std::string>(0, "")));

	// for (unsigned i = 0; i < rodIndex.size(); i++)
	// {
	// 	for(unsigned x = 0; x < rodIndex[i].size(); x++)
	// 	{
	// 		int startNode = std::stoi(rodIndex[i][x].substr(2,2));

	// 		for (unsigned j = 0; j < usedNodes.size(); j++)
	// 		{
	// 			//Begins by moving down river that rod is on
	// 			// std::cout << startNode << std::endl;
	// 			paths[currentIndex][j].push_back(rodIndex[i][x]);
	// 			//Set up a pointer
	// 			// std::cout << rodIndex[i][x] << std::endl;
	// 			// std::cout << startNode << " to " << usedNodes[j] << std::endl;
	// 			std::vector<int> *ptrShortPath = &shortestPaths[startNode][usedNodes[j]];

	// 			for (unsigned k = 0; k < (*ptrShortPath).size() - 1; k++)
	// 			{
	// 				if ((*ptrShortPath)[k] != -1)
	// 				{
	// 					// std::cout << "Hello";
	// 					std::string point1 = std::to_string((*ptrShortPath)[k]);
	// 					std::string point2 = std::to_string((*ptrShortPath)[k+1]);

	// 					if (point1.length() == 1)
	// 					{
	// 						point1 = "0" + point1;
	// 					}
	// 					if (point2.length() == 1)
	// 					{
	// 						point2 = "0" + point2;
	// 					}

	// 					std::string connection = point1 + point2;

	// 					// std::cout << connection << " ";

	// 					paths[currentIndex][j].push_back(connection);
	// 				}
	// 			}

	// 			// std::cout << std::endl;
	// 		}

	// 		for (unsigned k  = 0; k <  shortestPaths[startNode][usedNodes[99]].size() - 1; k++) 
	// 		{
	// 			paths[currentIndex][usedNodes.size()].push_back(rodIndex[i][x]);
	// 			if (shortestPaths[startNode][usedNodes[99]][k] != -1)
	// 			{
	// 				std::string point1 = std::to_string(shortestPaths[startNode][usedNodes[99]][k]);
	// 				std::string point2 = std::to_string(shortestPaths[startNode][usedNodes[99]][k+1]);

	// 				if (point1.length() == 1)
	// 				{
	// 					point1 = "0" + point1;
	// 				}
	// 				if (point2.length() == 1)
	// 				{
	// 					point2 = "0" + point2;
	// 				}

	// 				std::string connection = point1 + point2;

	// 				// std::cout << connection << " ";

	// 				paths[currentIndex][usedNodes.size()].push_back(connection);
	// 			}
	// 		}

	// 		currentIndex++;
	// 	}
	// }


	// //Find shortest path and only use that one
	// for (unsigned i = 0; i < rodIndexSize; i++)
	// {
	// 	int minLength = -1;
	// 	int index = 0;

	// 	for (unsigned j = 0; j < usedNodes.size() + 1; j++)
	// 	{
	// 		if (minLength == -1 && paths[i][j].size() > 1)
	// 		{
	// 			minLength = paths[i][j].size();
	// 			index = j;
	// 		}

	// 		else if(paths[i][j].size() > 1)
	// 		{
	// 			std::cout << paths[i][j].size() << " " << minLength <<std::endl;
	// 			if(paths[i][j].size() < minLength)
	// 			{
					
	// 				minLength = paths[i][j].size();
	// 				index = j;
	// 			}
	// 		}
	// 	}


	// 	// std::cout << "Crash After" << std::endl;

	// 	for (unsigned k = 0; k < paths[i][index].size(); k++)
	// 	{
	// 		// std::cout << index << std::endl;
	// 		connectionsFile << paths[i][index][k] << " ";
	// 	}

	// 	// std::cout << "Crash before" << std::endl;

	// 	connectionsFile << std::endl;
	// }




	//Loop through each node in the input connections
	// for (unsigned i = 0; i < usedNodes.size(); i++)
	// {
		
	// 	for (unsigned j = 0; j < usedNodes.size(); j++)
	// 	{
	// 		//Create a pointer so that it is easier to read
			
	// 		std::vector<int> *ptrShortPath = &shortestPaths[usedNodes[i]][usedNodes[j]];
	// 		// std::cout << "After";
	// 		//Loop through each shortest path
	// 		bool newLineNeeded = false;


	// 		std::cout << usedNodes[i] << " " << usedNodes[j] << std::endl;

	// 		// std::cout << "Break before?" << std::endl;
	// 		for(unsigned k = 0; k < shortestPaths[usedNodes[i]][usedNodes[j]].size() - 1; k++)
	// 		{
	// 			//If the two nodes are connected then set the connection matrix to show that they are
	// 			if ((*ptrShortPath)[k] != -1)
	// 			{
	// 				// connectionsMatrix[(*ptrShortPath)[k]][(*ptrShortPath)[k+1]] = 1;
	// 				std::string point1 = std::to_string((*ptrShortPath)[k]);
	// 				std::string point2 = std::to_string((*ptrShortPath)[k+1]);

	// 				if (point1.length() == 1)
	// 				{
	// 					point1 = "0" + point1;
	// 				}
	// 				if (point2.length() == 1)
	// 				{
	// 					point2 = "0" + point2;
	// 				}

	// 				std::string connection = point1 + point2;

	// 				// connectionsFile << connection << " ";

	// 				std::cout << connection << " ";

	// 				paths[i][j].push_back(connection);

	// 				newLineNeeded = true;

	// 				// for ( unsigned x = 0; x < connectionsUsed.size(); x++)
	// 				// {
	// 				// 	std::string node1Connect = connectionsUsed[x].substr(0, 2);
	// 				// 	std::string node2Connect = connectionsUsed[x].substr(2, 2);

	// 				// 	if (point1.compare(node1Connect) == 0 && point2.compare(node2Connect) != 0)
	// 				// 	{	
	// 				// 		std::cout << "Found" << std::endl;
	// 				// 		if (std::find(newNodes.begin(), newNodes.end(), (*ptrShortPath)[k]) == newNodes.end())
	// 				// 		{
	// 				// 			newNodes.push_back((*ptrShortPath)[k]);
	// 				// 		}
	// 				// 	} // else {
	// 				// 	// 	connectionsUsed.push_back(connection);
	// 				// 	// }
	// 				// }


	// 				// if (std::find(connectionsUsed.begin(), connectionsUsed.end(), connection) == connectionsUsed.end())
	// 				// {
	// 				// 	std::cout << "Here";
	// 				// 	connectionsUsed.push_back(connection);
	// 				// }
	// 				// 	newNodes.push_back((*ptrShortPath)[k]);
	// 				// }
	// 			}
	// 		}

	// 		if (newLineNeeded)
	// 		{
	// 			// connectionsFile << std::endl; //(*ptrShortPath)[(*ptrShortPath).size() - 1] << std::endl;
	// 			std::cout << std::endl;	
	// 		}
	// 		// std::cout << "Break after?" << std::endl;		
	// 	}
	// }

	//Check path to last node
	// for (unsigned i = 0; i < usedNodes.size(); i++)
	// {
	// 	for (unsigned j = 0; j < shortestPaths[usedNodes[i]][99].size() - 1; j++)
	// 	{
	// 		if (shortestPaths[usedNodes[i]][99][j] != -1)
	// 			{
	// 				// connectionsMatrix[(*ptrShortPath)[k]][(*ptrShortPath)[k+1]] = 1;
	// 				std::string point1 = std::to_string(shortestPaths[usedNodes[i]][99][j]);
	// 				std::string point2 = std::to_string(shortestPaths[usedNodes[i]][99][j+1]);

	// 				if (point1.length() == 1)
	// 				{
	// 					point1 = "0" + point1;
	// 				}
	// 				if (point2.length() == 1)
	// 				{
	// 					point2 = "0" + point2;
	// 				}

	// 				std::string connection = point1 + point2;

	// 				// connectionsFile << connection << " ";

	// 				paths[i][usedNodes.size()].push_back(connection);
	// 			}
	// 	}
	// }

	// //Find shortest path and only use that one
	// for (unsigned i = 0; i < usedNodes.size(); i++)
	// {
	// 	int minLength = -1;
	// 	int index = -1;

	// 	for (unsigned j = 0; j < usedNodes.size() + 1; j++)
	// 	{
	// 		if (minLength == -1 && paths[i][j].size() != 0)
	// 		{
	// 			minLength = paths[i][j].size();
	// 			index = j;
	// 		}

	// 		else if(paths[i][j].size() != 0)
	// 		{
	// 			std::cout << paths[i][j].size() << " " << minLength <<std::endl;
	// 			if(paths[i][j].size() < minLength)
	// 			{
					
	// 				minLength = paths[i][j].size();
	// 				index = j;
	// 			}
	// 		}
	// 	}

	// 	for (unsigned k = 0; k < paths[i][index].size(); k++)
	// 	{
	// 		connectionsFile << paths[i][index][k] << " ";
	// 	}

	// 	connectionsFile << std::endl;
	// }
	// //Open an output file stream and direct it to the correct file
	// std::ofstream outputHeightMap;
	// outputHeightMap.open("../../RiversofHanoi/Content/models/connections.txt");

	// //Loop through every used node and all its possible connections
	// for ( unsigned i = 0; i < shortestPaths.size(); i++)
	// {
	// 	for ( unsigned j = 0; j < shortestPaths[i].size(); j++)
	// 	{
	// 		//Output the start node and which node it'll go to
	// 		// outputHeightMap << usedNodes[i] << " to " << j << " : ";
	// 		//Loop through the path
	// 		for (unsigned k = 0; k < shortestPaths[i][j].size(); k++)
	// 		{
	// 			//Print the path
	// 			outputHeightMap << shortestPaths[i][j][k] << " ";
	// 		}

	// 		outputHeightMap << std::endl << std::endl;
	// 	}
	// }


	// // For loop to print which nodes are connected
	// for ( unsigned i = 0; i < connections.size(); i++)
	// {
	// 	for ( unsigned j = 0; j < connections[i].size(); j++)
	// 	{
	// 		outputHeightMap << usedNodes[i] << " to " << usedNodes[connections[i][j]] << " " << std::endl;
	// 	}

	// 	outputHeightMap << std::endl << std::endl;
	// }
}

/* Function to find the new intersections in the graph and create the connections matrix
 * and also to remove the edges that from a new source or sink node
 */
void findNewNodesAndRemoveEdges(std::vector< std::vector< std::vector<int> > > shortestPaths, std::vector<std::vector<int> > coordinates, std::vector< std::vector<int> > connections, std::vector< int > usedNodes, std::vector<std::vector<float> >* weightMatrix)
{
	//Open output file stream and direct it to correct file
	// std::ofstream outputGraph;
	// outputGraph.open("graphs.txt");

	// std::ofstream connectionsFile;
	// connectionsFile.open("../../RiversofHanoi/Content/models/connections.txt");

	//Initialise a matrix to be used to store all connections
	std::vector<std::vector<int> > connectionsMatrix( shortestPaths[0].size(), std::vector<int> (shortestPaths[0].size(), 0));

	//Create variables to be used
	std::vector<int> newNodes;
	std::vector<std::string> connectionsUsed;

	//Loop through each node in the input connections
	for (unsigned i = 0; i < connections.size(); i++)
	{
		for (unsigned j = 0; j < connections[i].size(); j++)
		{
			//Create a pointer so that it is easier to read
			std::vector<int> *ptrShortPath = &shortestPaths[usedNodes[i]][usedNodes[connections[i][j]]];
			//Loop through each shortest path
			bool newLineNeeded = false;

			for(unsigned k = 0; k < shortestPaths[usedNodes[i]][usedNodes[connections[i][j]]].size() - 1; k++)
			{
				//If the two nodes are connected then set the connection matrix to show that they are
				if ((*ptrShortPath)[k] != -1)
				{
					connectionsMatrix[(*ptrShortPath)[k]][(*ptrShortPath)[k+1]] = 1;

					//Get the two points that are connected as strings
					std::string point1 = std::to_string((*ptrShortPath)[k]);
					std::string point2 = std::to_string((*ptrShortPath)[k+1]);

					//Variables to store how many zeroes should be added to beginning of name
					int addPoints1 = 0;
					int addPoints2 = 0;

					//Make the point name the same length as the node names
					if (point1.length() != std::to_string(NUM_POINTS).length());
					{
						addPoints1 = (std::to_string(NUM_POINTS).length()) - (point1.length());
						//point1 = "0" + point1;
					}
					//Make the point name the same length as the node names
					if (point2.length() == 1)
					{
						addPoints2 = (std::to_string(NUM_POINTS).length()) - (point2.length());
						//point2 = "0" + point2;
					}

					//Add zeroes to beginning of name
					for (int z = 0; z < addPoints1; z++)
					{
						point1 = "0" + point1;
					}
					for (int z = 0; z < addPoints2; z++)
					{
						point2 = "0" + point2;
					}

					//Create the river name by adding the two point names together
					std::string connection = point1 + point2;

					//Get the length of the node name
					int riverNameLength = std::to_string(NUM_POINTS).length();

					newLineNeeded = true;

					//Loop through the connections
					for ( unsigned x = 0; x < connectionsUsed.size(); x++)
					{
						//Get the names of the nodes in the connections
						std::string node1Connect = connectionsUsed[x].substr(0, riverNameLength);
						std::string node2Connect = connectionsUsed[x].substr(riverNameLength, riverNameLength);

						//Check if the first node is the same as the first node in the river
						//And if the second node is different than the second node in the river
						if (point1.compare(node1Connect) == 0 && point2.compare(node2Connect) != 0)
						{	
							//If this is true then there is more than one river leaving the node
							//Therefore it is an intersection node
							//And should be a new node

							//Check so that the node is not already in the array
							if (std::find(newNodes.begin(), newNodes.end(), (*ptrShortPath)[k]) == newNodes.end())
							{
								//If it is not in the array then add it to the array.
								newNodes.push_back((*ptrShortPath)[k]);
							}
						} // else {
						// 	connectionsUsed.push_back(connection);
						// }
					}

					//Check so that the connection checked is not in the usedConnections list
					if (std::find(connectionsUsed.begin(), connectionsUsed.end(), connection) == connectionsUsed.end())
					{
						//Add it to the array if it is not already in the area
						connectionsUsed.push_back(connection);
					}
					// 	newNodes.push_back((*ptrShortPath)[k]);
					// }
				}
			}

			if (newLineNeeded)
			{
				// connectionsFile << std::endl; //(*ptrShortPath)[(*ptrShortPath).size() - 1] << std::endl;	
			}		
		}
	}


	//Find if any nodes are either starting from somewhere they shouldn't
	//Or ending somehwere they shouldn't
	//Only want top left to start and bottom-right to end
	std::vector<int> endNodes(NUM_POINTS * NUM_POINTS, 0);
	std::vector<int> startNodes(NUM_POINTS * NUM_POINTS, 0);

	int riverNameLength = std::to_string(NUM_POINTS).length();

	//Loop over the connections used
	for ( unsigned i = 0; i < connectionsUsed.size(); i++)
	{
		//Find the name of the start and end node
		std::string startNode = connectionsUsed[i].substr(0, riverNameLength);
		std::string endNode = connectionsUsed[i].substr(riverNameLength, riverNameLength);

		//These are used to count how many connections each point has
		//Either as a start node of a river
		startNodes[std::stoi(startNode)]++;
		//Or as an end node of a river
		endNodes[std::stoi(endNode)]++;
	}

	//This algorithm works by checking if a point is a start node of a river, but has no end nodes. This means it is a source and should be removed
	// Or if it is an end node, but is not a start node. this would mean that it is a sink.

	std::vector<int> removeStart;
	std::vector<int> connectEnd;
	std::vector<std::string> nodeStack;

	//Loop through all the point
	for (unsigned i = 0; i < startNodes.size(); i++)
	{
		//Check if the point is a start point fro a river, but not an end point for a river
		if ((endNodes[i] == 0 && startNodes[i] > 0)) //|| (startNodes[i] == 0 && endNodes[i] > 0))
		{
			//Add the point to the array that contains the points that should not be added to the graph
			removeStart.push_back(i);
			//Get the name of the start node
			std::string startNode = std::to_string(i);

			//Add zeroes to the beginning of the name, so that it is the same length
			int addStartNode = 0;

			if (startNode.length() != std::to_string(NUM_POINTS).length())
			{
				addStartNode = (std::to_string(NUM_POINTS).length()) - (startNode.length());
			}

			for (int z = 0; z < addStartNode; z++)
			{
				startNode = "0" + startNode;
			}

			//Variables to check when to stop the algorithm
			bool endFound = false;
			bool isStackEmpty = false;

			//While there is something is on the stack
			//This is used to deal with intersections coming from the source node
			while(!isStackEmpty)
			{
				//While the end of the river has not been found
				while(!endFound)
				{
					//Variable to hold the number of iterations since the last river was found
					int iter = 0;
					//Loop through all the connections
					for (unsigned j = 0; j < connectionsUsed.size(); j++)
					{
						//Get the names
						std::string node1Connect = connectionsUsed[j].substr(0, 2);
						std::string node2Connect = connectionsUsed[j].substr(2, 2);

						//Compare the the two start nodes to make sure that they are the same
						//And also check so that the point is not the top-left point
						if(startNode.compare(node1Connect) == 0 && node1Connect != "00")
						{
							//Check so that the point is only the end node of the river being removed
							// And check so that the point has not already been removed
							if (!(endNodes[std::stoi(node1Connect)] > 1) &&  (std::find( removeStart.begin(), removeStart.end(), std::stoi(node2Connect)) == removeStart.end())) //endNodes[std::stoi(node2Connect)] == 1 &&
							{
								//Add the point to the array of points not being added to the graph
								removeStart.push_back(std::stoi(node1Connect));
								//Make the new start node the next point on the river
								startNode = node2Connect;

								//Variable to store how many connections there are from that point
								int numberConnections = 0;
								//Loop through all the connections
								for (unsigned x = 0; x < connectionsUsed.size(); x++)
								{
									//If the start point of the connection is that same as the previous start point
									if (connectionsUsed[x].substr(0,2) == node1Connect)
									{
										//Increment the variable
										numberConnections++;
									}
									//If there is more than 1 connection
									if (numberConnections > 1)
									{
										//Find the names of the connections
										for (unsigned x = 0; x < connectionsUsed.size(); x++)
										{
											//If the start point of the connection is that same as the previous start point
											if (connectionsUsed[x].substr(0,2) == node1Connect)
											{
												//Check so it's not already in stack
												if ((std::find( nodeStack.begin(), nodeStack.end(), connectionsUsed[x].substr(2,2)) == nodeStack.end()))
												{
													//Add the node to the stack
													nodeStack.push_back(connectionsUsed[x].substr(2,2));
													// std::cout << "NODE IN STACK " << nodeStack.back() << std::endl;
												}
											}
										}
									}
								}
								//reset iter variable
								iter = 0;
							}
							else {
								//End found if the point is an end point for more than one river
								endFound = true;
							}
						}

						iter++;
					}

					// std::cout << iter << " " << connectionsUsed.size() <<  std::endl;
					//If the all the connections has been looped through since last time a river was removed
					if (iter == connectionsUsed.size())
					{
						//Then the end has been found
						endFound = true;
					}
				}

				//Check if stack empty
				if (nodeStack.size() == 0)
				{
					isStackEmpty = true;
				} else {
					//If the stack is not empty, then start from the next point in the stack
					startNode = nodeStack.back();
					nodeStack.pop_back();
					endFound = false;
				}
			}

		}
		//Check if the point is a end point fro a river, but not an start point for a river
		if (startNodes[i] == 0 && endNodes[i] > 0)
		{
			//Add the point to the array that contains the points that should not be added to the graph
			removeStart.push_back(i);
			//Get the name of the start node
			std::string startNode = std::to_string(i);

			//Add zeroes to the beginning of the name, so that it is the same length
			int addStartNode = 0;

			if (startNode.length() != std::to_string(NUM_POINTS).length())
			{
				addStartNode = (std::to_string(NUM_POINTS).length()) - (startNode.length());
			}

			for (int z = 0; z < addStartNode; z++)
			{
				startNode = "0" + startNode;
			}

			//Variables to check when to stop the algorithm
			bool endFound = false;
			bool isStackEmpty = false;

			//While there is something is on the stack
			//This is used to deal with intersections coming from the source node
			while(!isStackEmpty)
			{
				//While the end of the river has not been found
				while(!endFound)
				{
					//Variable to hold the number of iterations since the last river was found
					int iter = 0;
					//Loop through all the connections					
					for (unsigned j = 0; j < connectionsUsed.size(); j++)
					{
						//Get the names
						std::string node1Connect = connectionsUsed[j].substr(0, 2);
						std::string node2Connect = connectionsUsed[j].substr(2, 2);


						//Compare the the two end nodes to make sure that they are the same
						//And also check so that the point is not the bottom-right point
						if(startNode.compare(node2Connect) == 0 && node2Connect != "99")
						{
							//Check so that the point is only the start node of the river being removed
							// And check so that the point has not already been removed
							if ((!(startNodes[std::stoi(node2Connect)] > 1)) && (std::find( removeStart.begin(), removeStart.end(), std::stoi(node1Connect)) == removeStart.end())) //endNodes[std::stoi(node2Connect)] == 1 &&
							{
								//Add the point to the array of points not being added to the graph
								removeStart.push_back(std::stoi(node2Connect));
								//Make the new start node the previous point on the river
								startNode = node1Connect;

								//Variable to store how many connections there are from that point
								int numberConnections = 0;
								//Loop through all the connections
								for (unsigned x = 0; x < connectionsUsed.size(); x++)
								{
									//If the end point of the connection is that same as the previous end point
									if (connectionsUsed[x].substr(2,2) == node2Connect)
									{
										//Increment the variable
										numberConnections++;
									}
									//If there is more than 1 connection
									if (numberConnections > 1)
									{
										//Find the names of the connections
										for (unsigned x = 0; x < connectionsUsed.size(); x++)
										{
											//If the end point of the connection is that same as the previous end point
											if (connectionsUsed[x].substr(2,2) == node2Connect)
											{
												//Check so it's not already in stack
												if ((std::find( nodeStack.begin(), nodeStack.end(), connectionsUsed[x].substr(0,2)) == nodeStack.end()))
												{
													//Add the found point to the stack
													nodeStack.push_back(connectionsUsed[x].substr(0,2));
													// std::cout << "NODE IN STACK " << nodeStack.back() << std::endl;
												}
											}
										}
									}
								}
								//If the all the connections has been looped through since last time a river was removed
								iter = 0;
							} else {
								//End found if the point is an end point for more than one river
								endFound = true;
							}
						}

						iter++;
					}

					// std::cout << iter << " " << connectionsUsed.size() <<  std::endl;
					//If the all the connections has been looped through since last time a river was removed
					if (iter == connectionsUsed.size())
					{
						endFound = true;
					}
				}
				//Check if stack empty
				if (nodeStack.size() == 0)
				{
					isStackEmpty = true;
				} else {
					//If the stack is not empty, then start from the next point in the stack
					startNode = nodeStack.back();
					nodeStack.pop_back();
					endFound = false;
				}
			}

		}
		// if (endNodes[i] == 1 && startNodes[i] == 0)
		// {
		// 	connectEnd.push_back(i);
		// }
	}


	//Legacy code

	// std::cout <<"Before";

	// std::cout << newNodes.size() << std::endl;

	

	// std::cout <<"After";

	// std::cout << "Before";

	// //Shortest Paths for new Nodes
	// std::vector< std::vector< std::vector<int> > > shortestPathsNew = dijkstra(connections, *weightMatrix, newNodes);

	// std::cout << "After";
	// //Loop through each node in the input connections
	// for (unsigned i = 0; i < connections.size(); i++)
	// {
	// 	for (unsigned j = 0; j < connections[i].size(); j++)
	// 	{
	// 		//Create a pointer so that it is easier to read
	// 		std::vector<int> *ptrShortPath = &shortestPaths[i][usedNodes[connections[i][j]]];
	// 		//Loop through each shortest path
	// 		bool newLineNeeded = false;

	// 		for(unsigned k = 0; k < shortestPaths[i][usedNodes[connections[i][j]]].size() - 1; k++)
	// 		{
	// 			//If the two nodes are connected then set the connection matrix to show that they are
	// 			if ((*ptrShortPath)[k] != -1)
	// 			{
	// 				std::string point1 = std::to_string((*ptrShortPath)[k]);
	// 				std::string point2 = std::to_string((*ptrShortPath)[k+1]);

	// 				if (point1.length() == 1)
	// 				{
	// 					point1 = "0" + point1;
	// 				}
	// 				if (point2.length() == 1)
	// 				{
	// 					point2 = "0" + point2;
	// 				}

	// 				std::string connection = point1 + point2;

	// 				connectionsFile << connection << " ";

	// 				newLineNeeded = true;
	// 			}
	// 		}

	// 		if (newLineNeeded)
	// 		{
	// 			connectionsFile << std::endl;
	// 		}
	// 	}
	// }

	// std::cout << "New Nodes" << std::endl;
	// for (unsigned i = 0; i < newNodes.size(); i++)
	// {
	// 	std::cout << newNodes[i] << std::endl;
	// }
	// std::cout << "Old Nodes" << std::endl;
	// for (unsigned i = 0; i < usedNodes.size(); i++)
	// {
	// 	std::cout << usedNodes[i] << std::endl;
	// }

	//For loop to print the connection matrix
	// for (unsigned i = 0; i < connectionsMatrix.size(); i++)
	// {
	// 	for (unsigned j = 0; j < connectionsMatrix[i].size(); j++)
	// 	{
	// 		outputGraph << connectionsMatrix[i][j] << " ";
	// 	}

	// 	outputGraph << std::endl;
	// }
	// outputGraph << std::endl << std::endl;

	//For loop to print the coordinates
	// for (unsigned i = 0; i < coordinates.size(); i++)
	// {
	// 	outputGraph << coordinates[i][0] << ", " << coordinates[i][1] << std::endl;
	// }

	//Print the height map
	printHeightMap(connectionsMatrix, coordinates, newNodes, shortestPaths, removeStart, connectEnd);//usedNodes);
}

/* Function that draws the rivers onto a height map and sends all the information to be output
 */
void printHeightMap(std::vector< std::vector<int> > connectionsMatrix, std::vector< std::vector<int> > coordinates, std::vector<int> usedNodes, std::vector<std::vector< std::vector<int > > > shortestPaths, std::vector<int> removeStart, std::vector<int> connectEnd)
{
	//Initialise a matrix of all 1, rivers will be drawn into this
	std::vector< std::vector<float> > heightMap(SQUARE_SIZE, std::vector<float> (SQUARE_SIZE, 1));

	//Initialise vectors of data that will be filled up as the rivers are drawn
	std::vector< std::vector<float> > rodLocations(usedNodes.size(), std::vector<float> (0, 0));
	std::vector< std::vector<int> > riverLocations;
	std::vector< std::vector<std::string> > rodIndex(usedNodes.size(), std::vector<std::string>(0, ""));
	std::vector< std::string> riverNames;

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
				int nodeIndex = -1;
				//Check if it is a node, if it is we place rod
				if(std::find(usedNodes.begin(), usedNodes.end(), i) != usedNodes.end())
				{
					nodeIndex = std::find(usedNodes.begin(), usedNodes.end(), i) - usedNodes.begin();
				}

				//Check so i is not in the list that contains lines that should not be drawn
				if ( std::find(removeStart.begin(), removeStart.end(), i) == removeStart.end() || i == 0)
				{
					//Bresenham's Line algorithm
					drawLine(&heightMap, coordinates[i][0], coordinates[j][0], coordinates[i][1], coordinates[j][1], nodeIndex, &rodLocations, &riverLocations, &rodIndex, i, j, &riverNames);
				} else {
					//std::cout << i << " " << j << std::endl;
				}
			}
		}
	}

	//Change the heightmap so that the "rivers" are not 1 pixel wide
	makeDitches(&heightMap);

	// //Open an output file stream and point it to the height map file
	// std::ofstream graphHeightMap;
	// graphHeightMap.open("../heightmaps/graphHeightMap.txt");

	// //Print the heightmap to this file
	// for (unsigned i = 0; i < heightMap.size(); i++)
	// {
	// 	for (unsigned j = 0; j < heightMap[i].size(); j++)
	// 	{
	// 		graphHeightMap << heightMap[i][j] << " ";
	// 	}

	// 	graphHeightMap << std::endl;
	// }

	//Find the directions that the rivers are coming from
	connectionsPrint(shortestPaths, usedNodes, rodIndex, riverNames);
	//Send all the data to be output
	terrainGen::generateTerrain(&heightMap, &usedNodes, &coordinates, &rodLocations, &riverLocations, SQUARE_SIZE, &rodIndex, &riverNames);
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
	// int RIVER_WIDTH = 50;
	int interval = RIVER_WIDTH/3;
	float ditchHeight = 0.33f;
	//Increase ksquared with RIVER_WIDTH
	// int RIVER_WIDTHSquared = RIVER_WIDTH * RIVER_WIDTH * RIVER_WIDTH * RIVER_WIDTH;
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
				for (int k = 0; k < RIVER_WIDTH; k++)
				{
					//Check so the coordinate is not out of bounds
					if (j + k < SQUARE_SIZE)
					{
						//Check how high the point should be made
						// 3 steps of height
						//the further away the point is for the centre of the river, the shallower the point will be
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
						//Check how high the point should be made
						// 3 steps of height
						//the further away the point is for the centre of the river, the shallower the point will be
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
					}
					//Check so the coordinate is not out of bounds
					if (i + k < SQUARE_SIZE)
					{
						//Check how high the point should be made
						// 3 steps of height
						//the further away the point is for the centre of the river, the shallower the point will be
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
					}
					//Check so the coordinate is not out of bounds
					if (i - k > 0)
					{
						//Check how high the point should be made
						// 3 steps of height
						//the further away the point is for the centre of the river, the shallower the point will be
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
					}
				}
			}
		}
	}

	//Change the old heightmap to the new one
	(*oldHeightMap) = heightMap;
}

/* Function that uses Bresenham's line algorithm to draw the river on the height map, and also finds the coordinates for the rivers and rods 
 */
void drawLine(std::vector< std::vector<float> > *heightMap, int x1, int x2, int y1, int y2, int nodeIndex, std::vector< std::vector< float> >* rodLocations, std::vector< std::vector<int> >* riverLocations, std::vector<std::vector<std::string> >* rodIndex, int node1, int node2, std::vector<std::string> *riverNames)
{
	//Get the name of the name of the river
	std::string rodIndexString1 = std::to_string(node1); // + std::to_string(node2);
	std::string rodIndexString2 = std::to_string(node2);

	//Add zeros to string to make it the same length as the river names
	int addRodIndexString1 = 0;
	int addRodIndexString2 = 0;

	if (rodIndexString1.length() != std::to_string(NUM_POINTS).length())
	{
		addRodIndexString1 = (std::to_string(NUM_POINTS).length()) - (rodIndexString1.length());
	}

	if (rodIndexString2.length() != std::to_string(NUM_POINTS).length())
	{
		addRodIndexString2 = (std::to_string(NUM_POINTS).length()) - (rodIndexString2.length());
	}

	for (int z = 0; z < addRodIndexString1; z++)
	{
		rodIndexString1 = "0" + rodIndexString1;
	}
	for (int z = 0; z < addRodIndexString2; z++)
	{
		rodIndexString2 = "0" + rodIndexString2;
	}

	//Add strings together to get name of river
	std::string rodIndexString = rodIndexString1 + rodIndexString2;

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

	//Vector hold river location
	std::vector<int> riverLocation;
	 
	//Loop over x
	int i = 0;
	for(int x=(int)x1; x<maxX; x++)
	{
		//Check so y isn't out of bounds
		if(y < maxY)
		{
			//Check if the gradient was steep or not
			//As this changes which order the y and x coordinates go
		    if(steep)
		    {
		    	//Draw the point as a 0 on matrix
		    	//As line goes here
		    	(*heightMap)[y][x] = 0;
		    	//If its first iteration then find river coordinates for one side
		    	if(i == 0)
		    	{
					//Find river coordinates by adding values to x coordinate
		    		riverLocation.push_back(y + RIVER_WIDTH);
		    		riverLocation.push_back(x); //- RIVER_WIDTH);

		    		riverLocation.push_back(y - RIVER_WIDTH);
		    		riverLocation.push_back(x); // - RIVER_WIDTH);

		    	}
		    	//If its the last iteration
		    	if(x == maxX - 1)
		    	{
		    		//Find river coordinates by adding values to x coordinate
		    		riverLocation.push_back(y - RIVER_WIDTH);
		    		riverLocation.push_back(x + 1); // + RIVER_WIDTH);

		    		riverLocation.push_back(y + RIVER_WIDTH);
		    		riverLocation.push_back(x + 1); // + RIVER_WIDTH);

		    		(*riverNames).push_back(rodIndexString);


		    	}
		    	//If its 25th iteration
		    	if(i == 25)
		    	{
		    		//Get coordinates at this point, for rod placement, if the point is a node
		    		if (nodeIndex != -1)
		    		{
		    			(*rodLocations)[nodeIndex].push_back(y);
		    			(*rodLocations)[nodeIndex].push_back(x);

	    				(*rodIndex)[nodeIndex].push_back(rodIndexString);

		    		}
		    	}
		    }
		    else
		    {
		    	//Draw the point as a 0 on matrix
		    	//As line goes here
		        (*heightMap)[x][y] = 0;
		        //If its first iteration then find river coordinates for one side
		    	if(i == 0)
		    	{
		    		//Find river coordinates by adding values to x coordinate
		    		riverLocation.push_back(x);  //- RIVER_WIDTH);
		    		riverLocation.push_back(y - RIVER_WIDTH);

		    		riverLocation.push_back(x); //- RIVER_WIDTH);
		    		riverLocation.push_back(y + RIVER_WIDTH);
		    	}
		    	//If its the last iteration
		    	if(x == maxX - 1)
		    	{
		    		//Find river coordinates by adding values to x coordinate
		    		riverLocation.push_back(x + 1); //+ RIVER_WIDTH);
		    		riverLocation.push_back(y + RIVER_WIDTH);

		    		riverLocation.push_back(x + 1);// + RIVER_WIDTH);
		    		riverLocation.push_back(y - RIVER_WIDTH);

		    		(*riverNames).push_back(rodIndexString);
		    	}
		    	//If its 25th iteration
		    	if(i == 25)
		    	{
		    		//Get coordinates at this point, for rod placement, if the point is a node
		    		if (nodeIndex != -1)
		    		{
		    			(*rodLocations)[nodeIndex].push_back(x);
		    			(*rodLocations)[nodeIndex].push_back(y);

	    				(*rodIndex)[nodeIndex].push_back(rodIndexString);
		    		}
		    	}
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
		i++;
	}

	//Add river too array of rivers
	if (riverLocation.size() != 0 )
	{
		(*riverLocations).push_back(riverLocation);	
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

			//Changes weighting of directions, so that rivers behave more naturally, as top-right point is higher than bottom-left
			float xWeighted = 0.6f;
			float yWeighted = 0.4f;

			//Find distances in x and y direction
			int xSquared = (coordinates[i][0] - coordinates[connectionPoint][0]) * (coordinates[i][0] - coordinates[connectionPoint][0]);
			int ySquared = (coordinates[i][1] - coordinates[connectionPoint][1]) * (coordinates[i][1] - coordinates[connectionPoint][1]);

			//Times the values by the weightings
			xWeighted *= xSquared;
			yWeighted *= ySquared;

			//Get the distance between the two points
			float distance = std::sqrt(float(xWeighted) + float(yWeighted));

			//Find the manhattan distance between the two points
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
 */
void findConnections(std::vector<std::vector<int> > *connections, std::vector<int> usedNodes, std::vector<std::vector<int> > coordinates)
{
	//Connect each point to 2 other random points
	//Then connect a random number of arcs, around numNodes/2
	std::vector<int> numConnections(NUM_NODES, 0);

	std::vector<int> distances;
	std::vector<int> distancesUnsorted;
	std::vector<int> order;

	//Don't let it connect to nodes on the edge apart from top left and bottom right
	std::vector<int> disallowedNodes;

	//Find the nodes on the edges and add them to disallowed list
	for (int i  = 0; i < NUM_POINTS; i++)
	{
		for (int j = 0; j < NUM_POINTS; j++)
		{
			if ( j == 0 || i == 0 || j == NUM_POINTS - 1 || i == NUM_POINTS - 1)
			{
				int currentPoint = (i * NUM_POINTS) + j;
				// std::cout << currentPoint << std::endl;
				if (!(currentPoint == 0 || currentPoint == (NUM_POINTS * NUM_POINTS) - 1))
				{
					disallowedNodes.push_back(currentPoint);
				}
			}
		}
	}

	//Loop through the used Nodes and find the distances between them
	for (unsigned i = 0; i < usedNodes.size(); i++)
	{
		distances.push_back(coordinates[usedNodes[i]][0] + coordinates[usedNodes[i]][1]);
		distancesUnsorted.push_back(coordinates[usedNodes[i]][0] + coordinates[usedNodes[i]][1]);
		// std::cout << usedNodes[i] << std::endl;
	}

	//Sort one of the distance vectors
	std::sort(distances.begin(), distances.end());

	//Loop through the used nodes
	for (unsigned i  = 0; i < usedNodes.size(); i++)
	{
		for (unsigned j = 0; j < usedNodes.size(); j++)
		{
			//Check which distances correspond to the other distance vector
			if (distances[i] == distancesUnsorted[j] && std::find(order.begin(), order.end(),j)==order.end())
			{
				//Have a order vector to keep track of which node corresponds to which distance
				order.push_back(j);
				// std::cout << distances[i] << " " << j << std::endl;
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

				//Loop through nodes
				for(unsigned k = 0; k < usedNodes.size(); k++)
				{
					//Check so node isn't comparing itself
					// And make sure it is not already connected to the node
					// And make sure it is not a disallowed node
					if (i != order[k] && std::find(nodeConnections.begin(), nodeConnections.end(),order[k])==nodeConnections.end() && 
						std::find(disallowedNodes.begin(), disallowedNodes.end(),usedNodes[order[k]])==disallowedNodes.end())
					{
						if (distances[k] > distancesUnsorted[i])
						{
							nodeConnections.push_back(order[k]);
							break;
						}
					}
				}

				numConnections[i]++;
			}
		}
		//Add the new connections to the total vector
		(*connections).push_back(nodeConnections);

	}

	// for (int i = 0; i < (*connections).size(); i++)
	// {
	// 	std::cout << i << " to: ";

	// 	for (int j = 0; j < (*connections)[i].size(); j++)
	// 	{
	// 		std::cout << (*connections)[i][j] << " ";
	// 	}

	// 	std::cout << std::endl;
	// }
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
	std::vector< std::vector< std::vector<int> > > shortestPaths(weightMatrix.size(), std::vector<std::vector<int> > (weightMatrix.size(), std::vector<int> (1, -1)));
	//loop through the entire connections matrix
	for (unsigned i = 0; i < weightMatrix.size(); i++)
	{
		for (unsigned j = 0; j < weightMatrix[i].size(); j++)
		{
			//Find shortest path from i to j
			int startNode = i; //usedNodes[i];
			int endNode = j; //usedNodes[connections[i][j]];

				// std::cout << i << " " << j << std::endl;

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
			shortestPaths[startNode][endNode] = {startNode};

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
							std::vector<int> testvector(1, -1);
							//Check to see if the backwards path has a path found already
							if (shortestPaths[y][i] == testvector)
							{
								shortestPaths[y][i] = tempVector;
							}
							// std::vector<int>::iterator iter = std::find(usedNodes.begin(), usedNodes.end(),y);

							// if(iter != usedNodes.end())
							// {
								
							// 	//If no path found then set path to the one found
							// 	int index = std::distance(usedNodes.begin(), iter);
							// 	// std::cout << "break before?" << std::endl;
							// 	// std::cout << y << std::endl;
							// 	// std::cout << index << " "<< usedNodes[i]<< std::endl;
							// 	//shortestPaths[index][usedNodes[i]] = tempVector;
							// 	shortestPaths[i][j] = tempVector;
							// 	// std::cout << "break after?" << std::endl;
							// }
							
						}
					}
				}
				
			}
		}
	}

	return shortestPaths;
}

/* Function to add the connections to the intial start graph.
 * these lines are added so each point is 8-conntected to the points around it
 */
std::vector< std::vector<int> > addLinesToGraph()
{
	//Vector to hold connections
	std::vector < std::vector <int> > connections;
	//Loop trhough all the points on the graph
	for (int i = 0; i < NUM_POINTS; i++)
	{
		for (int j = 0; j < NUM_POINTS; j++)
		{
			std::vector<int> connectedness;
			int currentPoint = (i * NUM_POINTS) + j;
			//Find 8 connectedness and add point to vector
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

/* Function to find the coordinates of each point
 */
std::vector<std::vector<int> > findNodes()
{
	//Vector to hold coordinates
	std::vector< std::vector<int> > coordinates;
	//Loop through each point
	for (int i = 0; i < (NUM_POINTS); i++)
	{
		for (int j = 0; j < (NUM_POINTS); j++)
		{
			//Add the coordinates to the vector
			coordinates.push_back(std::vector<int>{i, j});
		}
	}

	return coordinates;
}

/* Function to update the coordinates to the size of the terrain
 */
void updateCoordinates(std::vector< std::vector<int> > *coordinates)
{
	//int to hold the scale factor
	int scale = SQUARE_SIZE / NUM_POINTS;

	//Loop through coordinates
	for (int i = 0; i < coordinates->size() - 1; i++)
	{
		//Multiply the coordinates by the scale factor
		(*coordinates)[i][0] = (*coordinates)[i][0] * scale;
		(*coordinates)[i][1] = (*coordinates)[i][1] * scale;
	}

	//Set last point to be in bottom-right corner
	(*coordinates)[(coordinates->size()) - 1][0] = SQUARE_SIZE - 1;
	(*coordinates)[(coordinates->size()) - 1][1] = SQUARE_SIZE - 1;
}

/* Function to output the coordinates of each node
 */
void outputNodes(std::vector<int> usedNodes, std::vector<std::vector<int> > coordinates)
{
	//Open the output stream at the correct file
	std::ofstream nodeFile;
	//Temporary Value
	nodeFile.open("../../RiversofHanoi/Content/models/nodes.txt");

	//Loop through each node
	for (int i = 0; i < usedNodes.size(); i++)
	{
		//Output the coordinates of that node
		nodeFile << coordinates[usedNodes[i]][0] << "," << coordinates[usedNodes[i]][1] << std::endl;
	}
}


//Starts at 2nd node in connection
/* Greedy algorithm to find the shortest paths from a node to the next node.
 * this algorithm does not care which node it finds, as long as it finds one
 */
std::vector<std::string> greedyShortestPath(std::string startRiver, std::vector<int> nodes, std::vector<std::string> riverNames, std::vector<std::string> startNodes, std::vector<std::string> endNodes)
{
	//Variable to store whether it has found a node or not
	bool endNodeReached = false;

	//Vector to store the path
	std::vector<std::string> shortPath;

	//Add first river to path
	shortPath.push_back(startRiver);

	//String to hold the river name currently at
	std::string currentConnection;
	//Start with end node of the first river
	std::string currentNode = startRiver.substr(2,2);
	//Find end node in list of start nodes to find connection
	std::vector<std::string>::iterator iter = std::find(startNodes.begin(), startNodes.end(),currentNode);
	int index = std::distance(startNodes.begin(), iter);

	//While it has not found a node
	while(!endNodeReached)
	{
		// //
		// std::cout << riverNames[index] << std::endl;
		// std::cout << currentNode << std::endl;
		//Check if the current point is a node or not, or if its the bottom-right corner
		if(std::find(nodes.begin(), nodes.end(),std::stoi(currentNode)) != nodes.end() || std::stoi(currentNode) == 99)
		{
			// std::cout << "Found " << shortPath.size() << std::endl;
			// std::cout << currentNode << std::endl;	

			//If it is a node then set variable to true and break loop
			endNodeReached = true;
			break;
		}

		//Change variables to next river
		currentConnection = riverNames[index];
		currentNode = endNodes[index];
		//Add current river to the shortest path connection
		shortPath.push_back(currentConnection);
		//Check if node exists in the start Node array
		std::vector<std::string>::iterator iter = std::find(startNodes.begin(), startNodes.end(),currentNode);
		if (iter != startNodes.end())
		{
			//If it does find the index of the node
			index = std::distance(startNodes.begin(), iter);
			// std::cout << "Crash After?" << std::endl;
			//Set the current node to the new point
			currentNode = startNodes[index];
		} else {
			//If it is not found, then the next point must be bottom-right corner
			currentNode = "99";
		}
	}

	//Return the shortest path found
	return shortPath;
}