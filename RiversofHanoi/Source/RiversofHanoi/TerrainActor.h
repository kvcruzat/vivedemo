// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "RodActor.h"
#include "RiverActor.h"
#include "FlowerActor.h"
#include "ProceduralMeshComponent.h"
#include "TerrainActor.generated.h"

UCLASS()
class RIVERSOFHANOI_API ATerrainActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATerrainActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	// Called when the actor is created 
	virtual void PostActorCreated() override;

	// Arrays of vectors for terrain mesh and coordinate locations of rods and nodes
	TArray<FVector> vertices, normals, nodes, rods, rivers, riverNorms;
	TArray<int32> Triangles;

	// Arrays of node IDs and connections between rod and river
	TArray<FString> nodeIDs;
	TArray<FString> rodRiverConnection;

	// Array of actors
	TArray<ARiverActor*> riverArray;
	TArray<ARodActor*> rodArray;
	TArray<AFlowerActor*> flowerArray;

	// Subclassing for creating actors
	TSubclassOf<class ARodActor> RodActor;
	TSubclassOf<class AFlowerActor> FlowerActor;

	FString currentLevel;	// name of level the terrain is created in
	float offset, scale, zOffset, zScale;	// used to scale to terrain size

	// Procedural mesh and material for terrain
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh)
		UProceduralMeshComponent* terrainMesh;
	UMaterial* terrainMaterial;

	// used to compute the scale needed to transform coordinates and to transform
	virtual void calculateScale();
	FVector transformCoord(FVector coord);

	// Methods for the creation of terrain and other components of the game
	virtual void createTerrain();
    virtual void addRods();
	virtual void setRodLocations();
	virtual void addRivers();
	virtual void addFlowers();
	virtual void assignConnectionActors();
	virtual void setFlows();
	virtual void computeGoal();

};