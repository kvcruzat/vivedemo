# vivedemo
HTC Vive Tech Demo

## Installing Project

1. Download the project from GitHub. (https://github.com/kvcruzat/vivedemo)
2. Download and install Unreal Engine. (https://www.unrealengine.com/)
3. Launch Unreal Engine.
4. Click the Browse button in Unreal and navigate to ~/RiversofHanoi and open RiversofHanoi.uproject

## Compiling Project

1. Open up Level 1 by going to File->Open Level   and then selecting Level1 under the levels directory.
2. Make sure the terrain is in the level.
  1. If the terrain is not on the level then you must drag it in.
  2. This is done under the modes tab, that will be on the left side of the window in the default Unreal layout.
  3. Select the place tab if not selected, and then search for "Terrain Actor", this should produce one result.
  4. Drag this result into the game screen and the terrain should place itself down.
3. Repeat step 2 for every level.
4. To build the project go to File->Package Project->Windows->Windows (64-bit)
5. Choose where the project should build to.

## Running Game

1. To run the game navigate to the folder that it was built to.
2. Then there will be a file called RiversofHnaoi.exe, run this file.

## Make a New Level

1. Navigate to ~/Randomly Generate Graphs/terrainGen
2. Open a command line window here
3. In the command line type "make", if no make utility is installed then install one, for example CMake (https://cmake.org/)
4. After the project is compiled then type "graphGen.exe".
5. This will produce the required files in ~/RiversofHanoi/Content/models
6. To make a new level, navigate to ~/RiversofHanoi/Content/Levels
7. Copy Level1.umap to a new file and rename it to next sequential level.
8. Navigate to ./data and then copy and rename the folder "Level1" to the same name.
9. Navigate to ~/RiversofHanoi/Content/models and copy the following files
  * connections.txt
  * nodeConnections.txt
  * nodes.txt
  * rivers.txt
  * rodIndex.txt
  * rods.txt
  * terrain.m
10. Move these files to ~/RiversofHanoi/Content/Levels/data/levelx, where x is the number that it was renamed to before.
11. Open the level in Unreal Engine by going to File->Open Level and selecting the new level.
12. Check so that everything looks right and that the terrain model is in the game.
