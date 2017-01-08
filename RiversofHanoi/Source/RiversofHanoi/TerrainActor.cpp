// Fill out your copyright notice in the Description page of Project Settings.

#include "RiversofHanoi.h"
#include "TerrainActor.h"
#include "Util.h"

// Sets default values
ATerrainActor::ATerrainActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	// Create terrain mesh and find the grass material
	terrainMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("TerrainMesh"));
	static ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("Material'/Game/StarterContent/Materials/M_Ground_Grass.M_Ground_Grass'"));
	if (Material.Object != NULL) { terrainMaterial = (UMaterial*)Material.Object; }

	// Reference rod actor class for creation
	static ConstructorHelpers::FObjectFinder<UBlueprint> rodBlueprint(TEXT("Blueprint'/Game/VirtualRealityBP/Blueprints/BP_Rod.BP_Rod'"));
	if (rodBlueprint.Object) { RodActor = (UClass*)rodBlueprint.Object->GeneratedClass;	}
	
	// Reference flower acor class for creation
	static ConstructorHelpers::FObjectFinder<UBlueprint> flowerBlueprint(TEXT("Blueprint'/Game/VirtualRealityBP/Blueprints/BP_Flower.BP_Flower'"));
	if (flowerBlueprint.Object) { FlowerActor = (UClass*)flowerBlueprint.Object->GeneratedClass; }

}

// Called when the game starts or when spawned
void ATerrainActor::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ATerrainActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called after terrain is created
void ATerrainActor::PostActorCreated()
{	
	Super::PostActorCreated();
	if (!this->IsTemplate(RF_Transient)) {	// check actor is actually created in the editor
		UWorld* TheWorld = GetWorld();
		currentLevel = TheWorld->GetMapName() + "/"; // get level name
		UE_LOG(LogTemp, Warning, TEXT("# Level: %s"), *currentLevel);	

		createTerrain();	// create the terrain mesh

		this->SetActorScale3D(FVector(700, 700, 400));	// scale terrain and position to origin
		this->SetActorLocation(FVector(0, 0, 0));

		// read in node and rod coordinates data
		Util *util = new Util();
		util->readNodeData(currentLevel + "nodes.txt", nodes);
		nodes = util->nodes;
		util->readRodData(currentLevel + "rods.txt", rods);
		rods = util->rods;

		// create rods, rivers, flowers and compute level goal
		calculateScale();
		addRods();
		setRodLocations();
		addRivers();
		addFlowers();
		assignConnectionActors();
		computeGoal();
		setFlows();
	}
}

// Create terrain mesh
void ATerrainActor::createTerrain() {
	Util *util = new Util();

	// read in terrain data
	util->readData(currentLevel + "terrain.m", vertices, Triangles);
	vertices = util->verts;
	normals = util->norms;
	Triangles = util->triangs;
	
	// create mesh and set grass material
	terrainMesh->CreateMeshSection(0, vertices, Triangles, normals, TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), true);
	terrainMesh->SetMaterial(0, terrainMaterial);
}

// Create rod actors
void ATerrainActor::addRods()
{

	Util *util = new Util();
	util->readRodRiverData(currentLevel + "rodIndex.txt"); // get which river is connected to which rod
	rodRiverConnection = util->rodRiverConnection;

    UWorld* const World = GetWorld();
	int rodNum = 0;
	for (int32 Index = 0; Index < rods.Num(); ++Index)
	{
		if (rods[Index] != FVector(-1,-1,-1) ) {
			if (World)
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = this;
				SpawnParams.Instigator = Instigator;
				FString actorName = FString::FromInt(rodNum);
				ARodActor* Rod = World->SpawnActor<ARodActor>(RodActor, FVector(0,0,0), FRotator(0, 0, 0), SpawnParams);
				Rod->rodID = actorName;
				Rod->riverConnection = rodRiverConnection[Index];	// assign rod with river ID of connected river
				Rod->SetOwner(this);
				rodArray.Add(Rod);
				rodNum++;
			}
		}
		else {
			rodNum = 0;
		}
	}
}

// Method to calculate the transformation from coordinates from text file to world
void ATerrainActor::calculateScale() {
	//Bounds point mapping for box corners
	const FVector BoundsPointMapping[8] =
	{
		FVector(1, 1, 1),
		FVector(1, 1, -1),
		FVector(1, -1, 1),
		FVector(1, -1, -1),
		FVector(-1, 1, 1),
		FVector(-1, 1, -1),
		FVector(-1, -1, 1),
		FVector(-1, -1, -1)
	};
	
	// Get terrain actor bounding box
	const FBox EachActorBounds = this->GetComponentsBoundingBox(false); 

	const FVector BoxCenter = EachActorBounds.GetCenter();	// terrain bounding box center

	const FVector BoxExtents = EachActorBounds.GetExtent();	// terrain box extents

	FVector bottomLeftCorner = BoxCenter + (BoundsPointMapping[2] * BoxExtents);
	FVector topRightCorner = BoxCenter + (BoundsPointMapping[3] * BoxExtents);

	float maxHeight = (BoxCenter + (BoundsPointMapping[0] * BoxExtents)).Z;
	float minHeight = (BoxCenter + (BoundsPointMapping[1] * BoxExtents)).Z;
	float height = maxHeight - minHeight;
	float terrainMax = nodes[1].X;
	float coordMax = bottomLeftCorner.X * 2.0;
	
	// calculate scale and offset for X (Y is same) and Z
	scale = coordMax / terrainMax;
	offset = terrainMax / 2.0;
	zScale = height / nodes[0].Z;
	zOffset = 1 - nodes[0].Z;

}

// Helper method to transform a vector using computed scale and offsets
FVector ATerrainActor::transformCoord(FVector coord) {
	coord.X = (coord.X - offset) * scale;
	coord.Y = (coord.Y - offset) * scale;
	coord.Z = (coord.Z - zOffset) * zScale;
	return coord;
}

// Moves rods to their correct positions
void ATerrainActor::setRodLocations() {
	const FBox rodBounds = rodArray[0]->GetComponentsBoundingBox(false); 

	const FVector rodCenter = rodBounds.GetCenter();

	const FVector rodExtents = rodBounds.GetExtent();

	int rodIndex = 0;
	for (int i = 0; i < rods.Num(); i++) {
		if (rods[i] != FVector(-1, -1, -1)) {
			rods[i].X = (rods[i].X - offset) * scale;
			rods[i].Y = (rods[i].Y - offset) * scale;
			rods[i].Z = ((rods[i].Z - zOffset) * zScale);
			rodArray[rodIndex]->SetActorLocation(rods[i], false);
			rodIndex++;
		}
		
	}
	
}

// Method to create river actors 
void ATerrainActor::addRivers() {

	Util *util = new Util();
	util->readRiverData(currentLevel + "rivers.txt", rivers);	// read in array of river vertices
	rivers = util->rivers;
	util->readRiverConnectionsData(currentLevel + "connections.txt");	// read in river connections
	TArray<FString> connections = util->riverConnections;
	TArray<FString> riverIDs = util->riverIDs;

	for (int i = 0; i < rivers.Num(); i++) {
		rivers[i] = transformCoord(rivers[i]);
		//rivers[i].Z += 25.0f;
	}


	UWorld* const World = GetWorld();
	for (int32 Index = 0; Index < rivers.Num() / 4; ++Index)
	{
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = Instigator;
			ARiverActor* River = World->SpawnActor<ARiverActor>(FVector(0, 0, 0), FRotator(0, 0, 0), SpawnParams);

			River->riverID = riverIDs[Index];
			bool addNextRiver = false;
			bool newConnection = true;
			bool connectionFound = false;
			for (int river = 0; river < connections.Num(); river++) {
			if ((addNextRiver || connectionFound) && !connections[river].Contains(TEXT("-1"))) {
				River->riverConnections.Add(connections[river]);	// assign river connections
				connectionFound = true;
			}
			else { connectionFound = false; }

			if (newConnection && riverIDs[Index].Contains(connections[river])) {
				addNextRiver = true;
			}
			else {
				addNextRiver = false;
			}

			if (connections[river].Contains(TEXT("-1"))) { newConnection = true; }
			else { newConnection = false; }
		}
			// create river mesh using vertices array
			TArray<FVector> riverVertices;
			riverVertices.Add(rivers[(Index * 4)]);
			riverVertices.Add(rivers[(Index * 4) + 1]);
			riverVertices.Add(rivers[(Index * 4) + 2]);
			riverVertices.Add(rivers[(Index * 4) + 3]);

			River->createMesh(riverVertices);	// call function in river actor to create the mesh
			River->SetOwner(this);
			riverArray.Add(River);
		}
	}

}

// Method to create flower actors for each node
void ATerrainActor::addFlowers() {

	Util* util = new Util();
	util->readNodeConnectionsData(currentLevel + "nodeConnections.txt");	// read in data on location of rivers at each node
	TArray<FString> nodeRivers = util->nodeRiverConnection;

	TArray<FVector> transformedNodes;

	// Array of all possible locations of rivers around around each node
	TArray<FVector> possibleLocs;
	possibleLocs.Add(FVector(0, -70, 0));
	possibleLocs.Add(FVector(50, -50, 0));
	possibleLocs.Add(FVector(70, 0, 0));
	possibleLocs.Add(FVector(50, 50, 0));
	possibleLocs.Add(FVector(0, 70, 0));
	possibleLocs.Add(FVector(-50, 50, 0));
	possibleLocs.Add(FVector(-70, 0, 0));
	possibleLocs.Add(FVector(-50, -50, 0));

	TArray<FString> tempNodeIDs;
	TArray<FString> sortedNodeRivers;
	TArray<FVector> tempFlowerLocs;

	// create array of node IDs that require flowers
	int nodeIndex = 2;
	bool prevEmpty = false;
	for (int rodRiverIndex = 2; rodRiverIndex < rodRiverConnection.Num(); rodRiverIndex++) {
		FString nodeID = rodRiverConnection[rodRiverIndex].Mid(0, 2);
		if (!tempNodeIDs.Contains(nodeID)) {
			if (!nodeID.Contains(TEXT("-1"))) {
				tempNodeIDs.Add(nodeID);
				sortedNodeRivers.Add(nodeRivers[(rodRiverIndex - 2) / 2]);
				transformedNodes.Add(transformCoord(nodes[nodeIndex]));
			}
			else if (prevEmpty) {
				nodeIndex--;
				prevEmpty = false;
			}
			else {
				prevEmpty = true;
			}
			nodeIndex++;
		}
	}

	tempNodeIDs.RemoveAt(0);	// first node doesn't need flowers ( flow will always be the same at source )
	sortedNodeRivers.RemoveAt(0);
	transformedNodes.RemoveAt(0);
	
	tempNodeIDs.Add(TEXT("99"));	// manually add last node
	sortedNodeRivers.Add(TEXT("0 1 2 3 4 5 6"));
	transformedNodes.Add(transformCoord(FVector(nodes[1].X, nodes[1].Y, 0)));

	UWorld* const World = GetWorld();
	for (int32 Index = 0; Index < tempNodeIDs.Num(); ++Index)
	{

		TArray<FString> usedRivers;

		sortedNodeRivers[Index].ParseIntoArray(usedRivers, _T(" "), true);

		// Use random number generation to create flower actors at available locations around a node
		int freeRivers = 8 - usedRivers.Num();
		for (int locIndex = 0; locIndex < possibleLocs.Num(); locIndex++) {
			if (!usedRivers.Contains(FString::FromInt(locIndex))) {
				int randNum = FMath::RandRange(0, 2);
				if (World)
				{
					if (freeRivers > possibleLocs.Num() - (locIndex + 1)) {
						if (randNum == 1) {
							FActorSpawnParameters SpawnParams;
							SpawnParams.Owner = this;
							SpawnParams.Instigator = Instigator;
							AFlowerActor* Flower = World->SpawnActor<AFlowerActor>(FlowerActor, FVector(0, 0, 0), FRotator(0, 0, 0), SpawnParams);
							Flower->SetOwner(this);
							Flower->nodeID = tempNodeIDs[Index];
							flowerArray.Add(Flower);
							tempFlowerLocs.Add(transformedNodes[Index] + possibleLocs[locIndex]);
							freeRivers--;
						}
					}
					else {
						FActorSpawnParameters SpawnParams;
						SpawnParams.Owner = this;
						SpawnParams.Instigator = Instigator;
						AFlowerActor* Flower = World->SpawnActor<AFlowerActor>(FlowerActor, FVector(0, 0, 0), FRotator(0, 0, 0), SpawnParams);
						Flower->SetOwner(this);
						Flower->nodeID = tempNodeIDs[Index];
						flowerArray.Add(Flower);
						tempFlowerLocs.Add(transformedNodes[Index] + possibleLocs[locIndex]);
						freeRivers--;
					}
					
				}
			}
		}
	}

	// Manually create flower actors for the last node
	if (World){
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = Instigator;
		AFlowerActor* Flower = World->SpawnActor<AFlowerActor>(FlowerActor, FVector(0, 0, 0), FRotator(0, 0, 0), SpawnParams);
		Flower->SetOwner(this);
		Flower->nodeID = TEXT("99");
		flowerArray.Add(Flower);
		tempFlowerLocs.Add(transformCoord(nodes[1]) + FVector(-50, -200, 0));

		AFlowerActor* Flower2 = World->SpawnActor<AFlowerActor>(FlowerActor, FVector(0, 0, 0), FRotator(0, 0, 0), SpawnParams);
		Flower2->SetOwner(this);
		Flower2->nodeID = TEXT("99");
		flowerArray.Add(Flower2);
		tempFlowerLocs.Add(transformCoord(nodes[1]) + FVector(-20, -150, 0));

		AFlowerActor* Flower3 = World->SpawnActor<AFlowerActor>(FlowerActor, FVector(0, 0, 0), FRotator(0, 0, 0), SpawnParams);
		Flower3->SetOwner(this);
		Flower3->nodeID = TEXT("99");
		flowerArray.Add(Flower3);
		tempFlowerLocs.Add(transformCoord(nodes[1]) + FVector(-35, -230, 0));
	}

	const FBox flowerBounds = flowerArray[0]->GetComponentsBoundingBox(false); //All Components 
	const FVector flowerExtents = flowerBounds.GetExtent();
	
	// Move flower actors to correct locations
	for (int flowerIndex = 0; flowerIndex < flowerArray.Num(); flowerIndex++) {
		FVector flowerLoc = tempFlowerLocs[flowerIndex];
		flowerLoc.Z += flowerExtents.Z;
		//flowerLoc.Z += 40.0f + (flowerExtents.Z * 2);
		flowerArray[flowerIndex]->SetActorLocation(flowerLoc);
	}
}

// Assign values to all the actors such as river connections and IDs
void ATerrainActor::assignConnectionActors() {
	for (int rodIndex = 0; rodIndex < rodArray.Num(); rodIndex++) {
		FString connectedRiver = rodArray[rodIndex]->riverConnection;
		for (int riverIndex = 0; riverIndex < riverArray.Num(); riverIndex++) {
			FString riverID = riverArray[riverIndex]->riverID;
			if (connectedRiver.Contains(riverID)) {
				rodArray[rodIndex]->connectedRiver = riverArray[riverIndex]; // assign each rod it's connected river actor
				rodArray[rodIndex]->nodeID = riverArray[riverIndex]->riverID.Mid(0, 2); // rod's node ID
				riverArray[riverIndex]->nodeID = riverArray[riverIndex]->riverID.Mid(0, 2);	// river's connected node ID
				break;
			}
		}
	}

	// populate array of node IDs
	for (int rodIndex = 0; rodIndex < rodArray.Num(); rodIndex++) {
		FString rodNode = rodArray[rodIndex]->nodeID;
		if (!nodeIDs.Contains(rodNode)) {
			nodeIDs.Add(rodNode);
		}
	}

	nodeIDs.Sort();

	if (!nodeIDs.Contains(TEXT("00"))) {
			nodeIDs.Insert(TEXT("00"),0);
			for (int riverIndex = 0; riverIndex < riverArray.Num(); riverIndex++) {
				if (riverArray[riverIndex]->riverID.Mid(0, 2).Contains(TEXT("00")) ) {
					riverArray[riverIndex]->nodeID = TEXT("00");
					break;
				}
			}
	}
	else if (!nodeIDs.Contains(TEXT("99"))) {
			nodeIDs.Add(TEXT("99"));
	}

	// Let the rods in each node know each other
	for (int rodIndex = 0; rodIndex < rodArray.Num(); rodIndex++) {
		for (int rodIndex2 = 0; rodIndex2 < rodArray.Num(); rodIndex2++) {
			if (rodArray[rodIndex]->nodeID.Contains(rodArray[rodIndex2]->nodeID) && rodArray[rodIndex] != rodArray[rodIndex2]
				&& !rodArray[rodIndex]->OtherRod->IsValidLowLevel() && !rodArray[rodIndex2]->OtherRod->IsValidLowLevel()) {
				rodArray[rodIndex]->OtherRod = rodArray[rodIndex2];
				rodArray[rodIndex2]->OtherRod = rodArray[rodIndex];
			}
		}
	}

	TArray<FString> flowerNodeIDs;

	for (int riverIndex = 0; riverIndex < riverArray.Num(); riverIndex++) {
		TArray<FString> riverConnections = riverArray[riverIndex]->riverConnections;

		if (riverConnections.Num() == 0) {
			for (int nodeIndex = 0; nodeIndex < nodeIDs.Num(); nodeIndex++) {
				if (riverArray[riverIndex]->riverID.Mid(2, 2).Contains(nodeIDs[nodeIndex]) ) {
					riverArray[riverIndex]->outputNode = nodeIDs[nodeIndex];	// assign each final river in the connection it's output node ID
					for (int river = 0; river < riverArray.Num(); river++) {
						if (riverArray[riverIndex]->outputNode.Contains(riverArray[river]->nodeID)) {
							riverArray[riverIndex]->outputRivers.Add(riverArray[river]);	// final river in connection know the rivers it will split to
						}
					}
				}
			}

			if (riverArray[riverIndex]->riverID.Mid(2, 2).Contains(TEXT("99"))) {
				riverArray[riverIndex]->outputNode = TEXT("99");
			}

			for (int rodIndex = 0; rodIndex < rodArray.Num(); rodIndex++) {
				if (riverArray[riverIndex]->outputNode.Contains(rodArray[rodIndex]->nodeID)) {
					rodArray[rodIndex]->inputRivers.Add(riverArray[riverIndex]); // each rod know the rivers it takes as input
				}
			}

			for (int flowerIndex = 0; flowerIndex < flowerArray.Num(); flowerIndex++) {
				if (riverArray[riverIndex]->outputNode.Contains(flowerArray[flowerIndex]->nodeID)) {
					riverArray[riverIndex]->flowerArray.Add(flowerArray[flowerIndex]);	// final river that leads to a node know all the flowers at node
				}
			}
		}
		else {
			for (int connection = 0; connection < riverConnections.Num(); connection++) {
				for (int riverIndex2 = 0; riverIndex2 < riverArray.Num(); riverIndex2++) {
					FString riverID = riverArray[riverIndex2]->riverID;
					if (riverConnections[connection].Contains(riverID)) {
						riverArray[riverIndex]->connectedRivers.Add(riverArray[riverIndex2]);	// starting river know its all connected rivers
						break;
					}
				}
			}
		}

		if (riverArray[riverIndex]->flowerArray.Num() > 0) {
			if (!flowerNodeIDs.Contains(riverArray[riverIndex]->outputNode)) {
				riverArray.Top()->nodeGoals.Add(false);	// Use last river to hold the goal completion status
				riverArray.Top()->flowerNodeIDs.Add(riverArray[riverIndex]->outputNode);	// Add flower node IDs
				flowerNodeIDs.Add(riverArray[riverIndex]->outputNode);	
			}
		}

	}

	for (int riverIndex = 0; riverIndex < riverArray.Num(); riverIndex++) {
		for (int riverIndex2 = 0; riverIndex2 < riverArray.Num(); riverIndex2++) {
			if (riverArray[riverIndex]->outputNode.Contains(riverArray[riverIndex2]->outputNode) && riverArray[riverIndex] != riverArray[riverIndex2]
				&& !riverArray[riverIndex]->overlappedRivers.Contains(riverArray[riverIndex2]) ) {
				riverArray[riverIndex]->overlappedRivers.Add(riverArray[riverIndex2]);	// Let overlapped rivers at a node know about each other
			}
		}

		if (riverArray[riverIndex]->flowerArray.Num() > 0) {
			riverArray[riverIndex]->finalNodeRiver = riverArray.Top();	// let all rivers that have flowers know about the final river
		}
	}
	
}

// Method to set initial flows
void ATerrainActor::setFlows() {

	float flow = 0;
	int numRods = 0;
	for (int rodIndex = 0; rodIndex < rodArray.Num(); rodIndex++) {
		if (nodeIDs[0].Contains(rodArray[rodIndex]->nodeID)) {
			++numRods;
		}
	}

	for (int riverIndex = 0; riverIndex < riverArray.Num(); riverIndex++) {
		if (nodeIDs[0].Contains(riverArray[riverIndex]->nodeID)) {
			flow = 24;
			if (numRods == 0) {
				riverArray[riverIndex]->changeFlow(flow);
			}

			if (numRods == 2) {
				flow = flow / 2;

				for (int rodIndex = 0; rodIndex < rodArray.Num(); rodIndex++) {
					if (nodeIDs[0].Contains(rodArray[rodIndex]->nodeID)) {
						rodArray[rodIndex]->connectedRiver->changeFlow(flow);
					}
				}
			}
			break;
		}
	}
}

// Method to simulate random discs and use as goal
void ATerrainActor::computeGoal() {

	float flow = 0;
	int numRods = 0;
	for (int rodIndex = 0; rodIndex < rodArray.Num(); rodIndex++) {
		if (nodeIDs[0].Contains(rodArray[rodIndex]->nodeID)) {
			++numRods;
		}
	}

	// Array of possible disc values a rod may have using flow change difference
	TArray<float> possibleDiscValues;
	possibleDiscValues.Add(0);
	possibleDiscValues.Add((-(1.0f / 6.0f)));
	possibleDiscValues.Add((-(1.0f / 3.0f)));
	possibleDiscValues.Add((-(1.0f / 2.0f)));
	possibleDiscValues.Add((-(5.0f / 6.0f)));
	possibleDiscValues.Add(-1);

	// randomly assign each rod with a possible disc value
	for (int rodIndex = 0; rodIndex < rodArray.Num(); rodIndex++) {
		rodArray[rodIndex]->connectedRiver->discStatus += possibleDiscValues[FMath::RandRange(0,possibleDiscValues.Num() - 1)];
		UE_LOG(LogTemp, Warning, TEXT("# Rod%s: %s"), *rodArray[rodIndex]->GetFName().ToString(), *FString::SanitizeFloat(rodArray[rodIndex]->connectedRiver->discStatus));
	}

	// simulate flows of rivers
	for (int riverIndex = 0; riverIndex < riverArray.Num(); riverIndex++) {
		if (nodeIDs[0].Contains(riverArray[riverIndex]->nodeID)) {
			flow = 24;
			if (numRods == 0) {
				riverArray[riverIndex]->changeFlow(flow);
			}

			if (numRods == 2) {
				flow = flow / 2;

				for (int rodIndex = 0; rodIndex < rodArray.Num(); rodIndex++) {
					if (nodeIDs[0].Contains(rodArray[rodIndex]->nodeID)) {
						float flowDiff = (flow + (flow * rodArray[rodIndex]->connectedRiver->discStatus));
						rodArray[rodIndex]->connectedRiver->changeFlow(flowDiff);
					}
				}
			}
			break;
		}
	}

	// Set required flows for each flower actor
	for (int riverIndex = 0; riverIndex < riverArray.Num(); riverIndex++) {
		if (nodeIDs.Contains(riverArray[riverIndex]->outputNode) && riverArray[riverIndex]->flowerArray.Num() != 0 ) {
			float totalFlow = riverArray[riverIndex]->flow;
			for (int overlappedIndex = 0; overlappedIndex < riverArray[riverIndex]->overlappedRivers.Num(); overlappedIndex++) {
				totalFlow += riverArray[riverIndex]->overlappedRivers[overlappedIndex]->flow;	// accumulate flow at a node
			}
			
			for (int flowerIndex = 0; flowerIndex < riverArray[riverIndex]->flowerArray.Num(); flowerIndex++) {
				riverArray[riverIndex]->flowerArray[flowerIndex]->requiredFlow = totalFlow;	// set required flow for each flower
			}
			UE_LOG(LogTemp, Warning, TEXT("# Node%s: %s"), *riverArray[riverIndex]->outputNode, *FString::SanitizeFloat(riverArray[riverIndex]->flowerArray[0]->requiredFlow));
		}
		// same as above but for last node
		else if (riverArray[riverIndex]->outputNode.Contains(TEXT("99")) && riverArray[riverIndex]->flowerArray.Num() != 0) {
			float totalFlow = riverArray[riverIndex]->flow;
			for (int overlappedIndex = 0; overlappedIndex < riverArray[riverIndex]->overlappedRivers.Num(); overlappedIndex++) {
				totalFlow += riverArray[riverIndex]->overlappedRivers[overlappedIndex]->flow;
			}

			for (int flowerIndex = 0; flowerIndex < riverArray[riverIndex]->flowerArray.Num(); flowerIndex++) {
				riverArray[riverIndex]->flowerArray[flowerIndex]->requiredFlow = totalFlow;
			}
		}
	}

	// reset disc status for each rod
	for (int rodIndex = 0; rodIndex < rodArray.Num(); rodIndex++) {
		rodArray[rodIndex]->connectedRiver->discStatus = 0;
	}

	// reset flows of every river
	for (int riverIndex = 0; riverIndex < riverArray.Num(); riverIndex++) {
		riverArray[riverIndex]->flow = 0;
	}
}
