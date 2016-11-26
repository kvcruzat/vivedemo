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
	util->readData("terrain.m", vertices, normals, Triangles);
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
		addRods();
		setRodLocations();
	}
}

void ATerrainActor::addRods()
{
    Util *util = new Util();
	util->readNodeData("nodes.txt", nodes);
	nodes = util->nodes;
	util->readRodData("rods.txt", rods);
    rods = util->rods;

	UE_LOG(LogTemp, Warning, TEXT("# RodNum: %s"), *FString::FromInt(rods.Num()));
	UE_LOG(LogTemp, Warning, TEXT("# Rod0: %s"), *rods[0].ToString());
	UE_LOG(LogTemp, Warning, TEXT("# Rod2: %s"), *rods[2].ToString());

    UWorld* const World = GetWorld();
	int nodeNum = 0;
	int rodNum = 0;
	for (int32 Index = 0; Index < rods.Num(); ++Index)
	{
		if (rods[Index] != FVector(-1,-1,-1) ) {
			if (World)
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = this;
				SpawnParams.Instigator = Instigator;
				FString actorName = FString(TEXT("N")) + FString::FromInt(nodeNum) + FString(TEXT("_R")) + FString::FromInt(rodNum);
				ARodActor* Rod = World->SpawnActor<ARodActor>(RodActor, FVector(0,0,0), FRotator(0, 0, 0), SpawnParams);
				Rod->rodID = actorName;
				Rod->SetOwner(this);
				rodArray.Add(Rod);
				rodNum++;
			}
		}
		else {
			nodeNum++;
			rodNum = 0;
		}
	}
}

void ATerrainActor::setRodLocations()
{
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

	const FBox rodBounds = rodArray[0]->GetComponentsBoundingBox(false); //All Components 

	const FVector rodCenter = rodBounds.GetCenter();	//Center

															//Extents
	const FVector rodExtents = rodBounds.GetExtent();

	

	float maxHeight = (BoxCenter + (BoundsPointMapping[0] * BoxExtents)).Z;
	float minHeight = (BoxCenter + (BoundsPointMapping[1] * BoxExtents)).Z;
	float height = maxHeight + minHeight;
	float terrainMax = nodes[1].X;
	float coordMax = bottomLeftCorner.X * 2.0;

	float scale = coordMax / terrainMax;
	float offset = terrainMax / 2.0;
	float zScale = height / nodes[0].Z;
	float zOffset = 1 - nodes[0].Z;

	UE_LOG(LogTemp, Warning, TEXT("# %s, %s, %s, %s"), *FString::SanitizeFloat(terrainMax), *FString::SanitizeFloat(coordMax), *FString::SanitizeFloat(scale), *FString::SanitizeFloat(offset));

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