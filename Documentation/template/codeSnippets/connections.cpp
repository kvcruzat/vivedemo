//Loop through the amount of nodes you want to add
for ( int i = 0; i < NUM_NODES; i++)
{
	//Vector to store connections
	std::vector<int> nodeConnections;
	//Loop through a two times, so that each node is connected to two nodes
	for (int j = 0; j < 2; j++)
	{
		//Check so node only has 2 connections coming out of it
		if (numConnections[i] < 2)
		{
			//Loop through all nodes
			for(unsigned k = 0; k < usedNodes.size(); k++)
			{
				//i != order[k] checks if node is the same
				//First find
				if (i != order[k] && std::find(nodeConnections.begin(), 
					nodeConnections.end(),order[k])==nodeConnections.end() && 
					std::find(disallowedNodes.begin(), disallowedNodes.end(),
						usedNodes[order[k]])==disallowedNodes.end())
				{
					if (distances[k] > distancesUnsorted[i])
					{
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