// Fill out your copyright notice in the Description page of Project Settings.

#include "RiversofHanoi.h"
#include "TerrainActor.h"
#include "Util.h"

#include "ProceduralMeshComponent.h"

// Sets default values
ATerrainActor::ATerrainActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Util *util = new Util();;
	util->readData("terrain.m", vertices, normals, Triangles);
	vertices = util->verts;
	normals = util->norms;
	Triangles = util->triangs;

	USphereComponent* SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	RootComponent = SphereComponent;

	UProceduralMeshComponent* terrainMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("TerrainMesh"));

	UE_LOG(LogTemp, Warning, TEXT("# Vertices: %s"), *FString::FromInt(vertices.Num()));
	UE_LOG(LogTemp, Warning, TEXT("# Faces: %s"), *FString::FromInt(Triangles.Num() / 3));
	UE_LOG(LogTemp, Warning, TEXT("# Normals: %s"), *FString::FromInt(normals.Num()));

	//terrainMesh->CreateMeshSection(1, vertices, Triangles, normals, UV0, vertexColors, tangents, true);
	terrainMesh->CreateMeshSection(0, vertices, Triangles, normals, TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), true);

	static ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("Material'/Game/StarterContent/Materials/M_Ground_Grass.M_Ground_Grass'"));

	if (Material.Object != NULL) {
		UMaterial* terrainMaterial = (UMaterial*)Material.Object;
		terrainMesh->SetMaterial(0, terrainMaterial);
	}

	terrainMesh->SetupAttachment(RootComponent);

    addRods();
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

void ATerrainActor::addRods()
{
    Util *util = new Util();
    util->readNodeData("nodes.txt", nodes);

    nodes = util->nodes;

    UE_LOG(LogTemp, Warning, TEXT("# Nodes: %s"), *FString::FromInt(nodes.Num()));

    UWorld* const World = GetWorld();
    //for (int32 Index = 0; Index < nodes.Num(); ++Index)
    //{
        if (World)
        {
            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = this;
            SpawnParams.Instigator = Instigator;
            ARodActor* Rod = World->SpawnActor<ARodActor>(ARodActor::StaticClass(), FVector(0, 0, 0), FRotator(0, 0, 0), SpawnParams);
            Rod->SetActorLocation(this->GetActorLocation(), false);
            Rod->SetOwner(this);
        }
        //rods.Add(NewObject<ARodActor>(*this, *ARodActor));
        //rods[Index]->SetActorLocation(FVector(0, 0, 0), false);
    //}
}

void ATerrainActor::PostInitializeComponents()
{
    //addRods();
}


