//Function that reads in a terrain .txt file and outputs it as a model file
void Utils::readTerrainFileAndOutputM(std::string fileName) {
	std::string line;
	//Loads input file from parameters
	std::ifstream textFile((fileName).c_str());
	//Create output file
	std::ofstream modelFile("../models/terrain.m");

	//Vector for storing the split line
	std::vector<std::string> linevector;
	//Vector that stores all the terrain coordinates read in
	std::vector< std::vector<std::string> > terrainCoords;
	//Variabels to keep track of amount of rows and lines in txt file
	int numLines = 0;
	int numCols = 0;

	//Check if file is open
	if (textFile.is_open())
	{	
		//Read line by line
		while (std::getline(textFile, line))
		{
			//Clear vector
			linevector.clear();
			//Split line
			linevector = split(line, " ");
			//Store split line in terraincoords vector
			terrainCoords.push_back(linevector);
			//Increment numLines
			numLines++;
		}
	}

	//Get number of columns from vector size
	numCols = linevector.size();
	//Close text file
	textFile.close();
	//Vectors to hold vertices and faces
	std::vector<float> vertices;
	std::vector<float> indices;
	//check if m file is open
	if(modelFile.is_open())
	{
		//variables to hold indices of faces
		int p1Index = 0;
		int p2Index = 0;
		int p3Index = 0;
		int p4Index = 0;
		//Loop through entire file
		for(int i = 0; i < numLines; i++)
		{
			for(int j = 0; j < numCols; j++)
			{
				//Add vertex to vector with y value of terrain txt
				vertices.push_back((j - (numCols / 2)) / (float)(numLines / 2.00f));
				vertices.push_back(atof(terrainCoords[i][j].c_str()));
				vertices.push_back((i - (numLines / 2)) / (float)(numCols / 2.00f));
			}
		}

		for(int i = 0; i < (numLines - 1); i++)
		{
			for(int j = 0; j < (numCols - 1); j++)
			{
				//Find indices of vertices needed
				p1Index = ((i * numCols) + j);
				p2Index = ((i * numCols) + j) + 1;
				p3Index = ((i * numCols) + j) + numCols;
				p4Index = ((i * numCols) + j) + 1 + numCols;

				//Add two faces for each square to indices vector
				indices.push_back(p1Index);
				indices.push_back(p3Index);
				indices.push_back(p4Index);

				indices.push_back(p1Index);
				indices.push_back(p4Index);
				indices.push_back(p2Index);
			}
		}

		//Write comments to file
		modelFile << "#" << std::endl << "# Created by Philip Nilsson" << 
		std::endl << "#" << std::endl;
		modelFile << "#" << std::endl << "# Terrain vertices=" << 
		vertices.size() << " faces=" << indices.size() / 3 << std::endl;

		//Loop through and write vertices to file
		for (int i = 0; i < vertices.size() / 3; i++)
		{
			modelFile << "Vertex " << i << " " << vertices[3 * i] << " " << 
			vertices[(3 * i) + 1] << " " << vertices[(3 * i) + 2]  << std::endl;
		}

		//Loop through and write indices to file
		for (int i = 0; i < indices.size() / 3; i++)
		{
			modelFile << "Face " << i << " " << indices[3 * i] << " " << 
			indices[(3 * i) + 1] << " " << indices[(3 * i) + 2]  << std::endl; 
		} 
	}
}