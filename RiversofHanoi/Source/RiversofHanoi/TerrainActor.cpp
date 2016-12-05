// Fill out your copyright notice in the Description page of Project Settings.

#include "RiversofHanoi.h"
#include "TerrainActor.h"
#include "Util.h"

// Sets default values
ATerrainActor::ATerrainActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	Util *util = new Util();
	util->readData("terrain.m", vertices, Triangles);
	vertices = util->verts;
	normals = util->norms;
	Triangles = util->triangs;

	terrainMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("TerrainMesh"));

	UE_LOG(LogTemp, Warning, TEXT("# Vertices: %s"), *FString::FromInt(vertices.Num()));
	UE_LOG(LogTemp, Warning, TEXT("# Faces: %s"), *FString::FromInt(Triangles.Num() / 3));
	UE_LOG(LogTemp, Warning, TEXT("# Normals: %s"), *FString::FromInt(normals.Num()));

	terrainMesh->CreateMeshSection(0, vertices, Triangles, normals, TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), true);

	static ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("Material'/Game/StarterContent/Materials/M_Ground_Grass.M_Ground_Grass'"));

	if (Material.Object != NULL) {
		UMaterial* terrainMaterial = (UMaterial*)Material.Object;
		terrainMesh->SetMaterial(0, terrainMaterial);
	}

	static ConstructorHelpers::FObjectFinder<UBlueprint> rodBlueprint(TEXT("Blueprint'/Game/VirtualRealityBP/Blueprints/BP_Rod.BP_Rod'"));
	if (rodBlueprint.Object) {
		RodActor = (UClass*)rodBlueprint.Object->GeneratedClass;
	}
	//RootComponent = terrainMesh;

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

void ATerrainActor::PostActorCreated()
{	
	Super::PostActorCreated();
	if (!this->IsTemplate(RF_Transient)) {
		this->SetActorScale3D(FVector(700, 700, 400));
		this->SetActorLocation(FVector(0, 0, 0));

		Util *util = new Util();
		util->readNodeData("nodes.txt", nodes);
		nodes = util->nodes;
		util->readRodData("rods.txt", rods);
		rods = util->rods;

		calculateScale();
		addRods();
		setRodLocations();
		addRivers();
		assignConnectionActors();
		setFlows();
	}
}

void ATerrainActor::addRods()
{
	UE_LOG(LogTemp, Warning, TEXT("# RodNum: %s"), *FString::FromInt(rods.Num()));
	UE_LOG(LogTemp, Warning, TEXT("# Rod0: %s"), *rods[0].ToString());
	UE_LOG(LogTemp, Warning, TEXT("# Rod2: %s"), *rods[2].ToString());

	Util *util = new Util();
	util->readRodRiverData("rodIndex.txt");

	TArray<FString> rodRiverConnection = util->rodRiverConnection;

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
				Rod->riverConnection = rodRiverConnection[Index];
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

void ATerrainActor::calculateScale() {
	//The Actor Bounds Point Mapping
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

	//Get Actor Bounds                
	const FBox EachActorBounds = this->GetComponentsBoundingBox(false); //All Components 

	const FVector BoxCenter = EachActorBounds.GetCenter();	//Center

															//Extents
	const FVector BoxExtents = EachActorBounds.GetExtent();

	FVector bottomLeftCorner = BoxCenter + (BoundsPointMapping[2] * BoxExtents);
	FVector topRightCorner = BoxCenter + (BoundsPointMapping[3] * BoxExtents);

	for (int i = 0; i < 8; ++i) {
		FVector vertex = BoxCenter + (BoundsPointMapping[i] * BoxExtents);
		UE_LOG(LogTemp, Warning, TEXT("# Vertex%s: %s"), *FString::FromInt(i), *vertex.ToString());
	}


	float maxHeight = (BoxCenter + (BoundsPointMapping[0] * BoxExtents)).Z;
	float minHeight = (BoxCenter + (BoundsPointMapping[1] * BoxExtents)).Z;
	float height = maxHeight + minHeight;
	float terrainMax = nodes[1].X;
	float coordMax = bottomLeftCorner.X * 2.0;

	scale = coordMax / terrainMax;
	offset = terrainMax / 2.0;
	zScale = height / nodes[0].Z;
	zOffset = 1 - nodes[0].Z;

}

FVector ATerrainActor::transformCoord(FVector coord) {
	coord.X = (coord.X - offset) * scale;
	coord.Y = (coord.Y - offset) * scale;
	coord.Z = (coord.Z - zOffset) * zScale;
	return coord;
}

void ATerrainActor::setRodLocations() {
	const FBox rodBounds = rodArray[0]->GetComponentsBoundingBox(false); //All Components 

	const FVector rodCenter = rodBounds.GetCenter();	//Center

														//Extents
	const FVector rodExtents = rodBounds.GetExtent();

	int rodIndex = 0;
	for (int i = 0; i < rods.Num(); i++) {
		if (rods[i] != FVector(-1, -1, -1)) {
			rods[i].X = (rods[i].X - offset) * scale;
			rods[i].Y = (rods[i].Y - offset) * scale;
			rods[i].Z = ((rods[i].Z - zOffset) * zScale) + rodExtents.Z;
			rodArray[rodIndex]->SetActorLocation(rods[i], false);
			rodIndex++;
		}
		
	}
	
}

void ATerrainActor::addRivers() {

	Util *util = new Util();
	util->readRiverData("rivers.txt", rivers);
	rivers = util->rivers;
	util->readRiverConnectionsData("connections.txt");
	TArray<FString> connections = util->riverConnections;
	TArray<FString> riverIDs = util->riverIDs;

	UE_LOG(LogTemp, Warning, TEXT("# riverNum: %s"), *FString::FromInt(rivers.Num()));
	UE_LOG(LogTemp, Warning, TEXT("# River0: %s"), *rivers[0].ToString());
	UE_LOG(LogTemp, Warning, TEXT("# River2: %s"), *rivers[2].ToString());

	for (int i = 0; i < rivers.Num(); i++) {
		rivers[i] = transformCoord(rivers[i]);
		rivers[i].Z += 25.0f;
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
					River->riverConnections.Add(connections[river]);
					connectionFound = true;
				}
				else { connectionFound = false; }

				if (newConnection && riverIDs[Index].Contains(connections[river]) ) {
					addNextRiver = true;
				}
				else {
					addNextRiver = false;
				}
				
				if (connections[river].Contains(TEXT("-1"))) { newConnection = true; }
				else { newConnection = false; }
			}

			TArray<FVector> riverVertices;
			riverVertices.Add(rivers[(Index * 4)]);
			riverVertices.Add(rivers[(Index * 4) + 1]);
			riverVertices.Add(rivers[(Index * 4) + 2]);
			riverVertices.Add(rivers[(Index * 4) + 3]);

			River->createMesh(riverVertices);
			River->SetOwner(this);
			riverArray.Add(River);
		}
	}

}


void ATerrainActor::assignConnectionActors() {
	for (int rodIndex = 0; rodIndex < rodArray.Num(); rodIndex++) {
		FString connectedRiver = rodArray[rodIndex]->riverConnection;
		for (int riverIndex = 0; riverIndex < riverArray.Num(); riverIndex++) {
			FString riverID = riverArray[riverIndex]->riverID;
			if (connectedRiver.Contains(riverID)) {
				rodArray[rodIndex]->connectedRiver = riverArray[riverIndex];
				rodArray[rodIndex]->nodeID = riverArray[riverIndex]->riverID.Mid(0, 2);
				riverArray[riverIndex]->nodeID = riverArray[riverIndex]->riverID.Mid(0, 2);
				break;
			}
		}
	}

	UWorld* const World = GetWorld();

	for (int rodIndex = 0; rodIndex < rodArray.Num(); rodIndex++) {
		FString rodNode = rodArray[rodIndex]->nodeID;
		if (!nodeIDs.Contains(rodNode)) {
			nodeIDs.Add(rodNode);
		}
	}

	nodeIDs.Sort();

	/*for (int nodeIndex = 0; nodeIndex < nodeIDs.Num(); nodeIndex++) {
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = Instigator;
			ANodeActor* Node = World->SpawnActor<ANodeActor>(FVector(0, 0, 0), FRotator(0, 0, 0), SpawnParams);
			Node->SetOwner(this);
			Node->nodeID = nodeIDs[nodeIndex];
			nodeArray.Add(Node);
		}
	}*/

	if (!nodeIDs.Contains(TEXT("00"))) {
		/*if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = Instigator;
			ANodeActor* Node = World->SpawnActor<ANodeActor>(FVector(0, 0, 0), FRotator(0, 0, 0), SpawnParams);
			Node->SetOwner(this);
			Node->nodeID = TEXT("00");
			nodeArray.Insert(Node, 0);*/
			nodeIDs.Insert(TEXT("00"),0);
			for (int riverIndex = 0; riverIndex < riverArray.Num(); riverIndex++) {
				if (riverArray[riverIndex]->riverID.Mid(0, 2).Contains(TEXT("00")) ) {
					riverArray[riverIndex]->nodeID = TEXT("00");
					break;
				}
			}
		//}
	}
	else if (!nodeIDs.Contains(TEXT("99"))) {
		/*if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = Instigator;
			ANodeActor* Node = World->SpawnActor<ANodeActor>(FVector(0, 0, 0), FRotator(0, 0, 0), SpawnParams);
			Node->SetOwner(this);
			Node->nodeID = TEXT("99");
			nodeArray.Add(Node);*/
			nodeIDs.Add(TEXT("99"));
		//}
	}

	for (int rodIndex = 0; rodIndex < rodArray.Num(); rodIndex++) {
		for (int rodIndex2 = 0; rodIndex2 < rodArray.Num(); rodIndex2++) {
			if (rodArray[rodIndex]->nodeID.Contains(rodArray[rodIndex2]->nodeID) && rodArray[rodIndex] != rodArray[rodIndex2]
				&& !rodArray[rodIndex]->OtherRod->IsValidLowLevel() && !rodArray[rodIndex2]->OtherRod->IsValidLowLevel()) {
				rodArray[rodIndex]->OtherRod = rodArray[rodIndex2];
				rodArray[rodIndex2]->OtherRod = rodArray[rodIndex];
			}
		}
	}

	for (int riverIndex = 0; riverIndex < riverArray.Num(); riverIndex++) {
		TArray<FString> riverConnections = riverArray[riverIndex]->riverConnections;
		for (int connection = 0; connection < riverConnections.Num(); connection++) {
			for (int riverIndex2 = 0; riverIndex2 < riverArray.Num(); riverIndex2++) {
				FString riverID = riverArray[riverIndex2]->riverID;
				if (riverConnections[connection].Contains(riverID)) {
					riverArray[riverIndex]->connectedRivers.Add(riverArray[riverIndex2]);
					break;
				}
			}
			if (connection == riverConnections.Num() - 1) {
				for (int nodeIndex = 0; nodeIndex < nodeIDs.Num(); nodeIndex++) {
					if (riverArray[riverIndex]->connectedRivers[connection]->riverID.Mid(2, 2).Contains(nodeIDs[nodeIndex])) {
						riverArray[riverIndex]->connectedRivers[connection]->outputNode = nodeIDs[nodeIndex];
						for (int river = 0; river < riverArray.Num(); river++) {
							if (riverArray[riverIndex]->connectedRivers[connection]->outputNode.Contains(riverArray[river]->nodeID)) {
								riverArray[riverIndex]->connectedRivers[connection]->outputRivers.Add(riverArray[river]);
							}
						}
					}
				}
				for (int rodIndex = 0; rodIndex < rodArray.Num(); rodIndex++) {
					if (riverArray[riverIndex]->connectedRivers[connection]->outputNode.Contains(rodArray[rodIndex]->nodeID)) {
						rodArray[rodIndex]->inputRivers.Add(riverArray[riverIndex]->connectedRivers[connection]);
					}
				}
			}
		}
	}

	for (int riverIndex = 0; riverIndex < riverArray.Num(); riverIndex++) {
		for (int riverIndex2 = 0; riverIndex2 < riverArray.Num(); riverIndex2++) {
			if (riverArray[riverIndex]->outputNode.Contains(riverArray[riverIndex2]->outputNode) && riverArray[riverIndex] != riverArray[riverIndex2]
				&& !riverArray[riverIndex]->overlappedRivers.Contains(riverArray[riverIndex2]) ) {
				riverArray[riverIndex]->overlappedRivers.Add(riverArray[riverIndex2]);
			}
		}
	}


	//for (int nodeIndex = 0; nodeIndex < nodeArray.Num(); nodeIndex++) {
	//	for (int rodIndex = 0; rodIndex < rodArray.Num(); rodIndex++) {
	//		if (nodeArray[nodeIndex]->nodeID.Contains(rodArray[rodIndex]->nodeID)) {
	//			nodeArray[nodeIndex]->connectedRods.Add(rodArray[rodIndex]);
	//		}
	//	}
	//}
	
}

void ATerrainActor::setFlows() {

	/*for (int nodeIndex = 0; nodeIndex < nodeIDs.Num(); nodeIndex++) {*/
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
							float flowDiff = flow - rodArray[rodIndex]->connectedRiver->flow;
							rodArray[rodIndex]->connectedRiver->changeFlow(flowDiff);
						}
					}
				}
				break;
			}
		}
	//}

	

	/*for (int nodeIndex = 0; nodeIndex < nodeArray.Num(); nodeIndex++) {
		nodeArray[nodeIndex]->computeFlows();
	}*/

	/*for (int nodeIndex = 0; nodeIndex < nodeArray.Num(); nodeIndex++) {
		int flow = 0;

		if (nodeIndex == 0) {
			flow = 24;
			if (nodeArray[nodeIndex]->connectedRods.Num() == 0) {
				nodeArray[nodeIndex]->outputRiver->changeFlow(flow);
			}
		}
		else {
			for (int river = 0; river < nodeArray[nodeIndex]->inputRivers.Num(); river++) {
				flow += nodeArray[nodeIndex]->inputRivers[river]->flow;
			}
		}	
	
		if (nodeArray[nodeIndex]->connectedRods.Num() == 2) {
			flow = flow / 2;
	
			for (int rodIndex = 0; rodIndex < nodeArray[nodeIndex]->connectedRods.Num(); rodIndex++) {
				int flowDiff = flow - nodeArray[nodeIndex]->connectedRods[rodIndex]->connectedRiver->flow;
				nodeArray[nodeIndex]->connectedRods[rodIndex]->connectedRiver->changeFlow(flowDiff);
			}
		}
	}*/
}