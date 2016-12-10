// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "RodActor.h"
#include "RiverActor.h"
#include "FlowerActor.h"
#include "LevelStartWidget.h"
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

	virtual void PostActorCreated() override;

    //Called after components are initialised

	FString currentLevel;

	TArray<FVector> vertices;
	TArray<FVector> normals;
	TArray<int32> Triangles;
    TArray<FVector> nodes;
	TArray<FVector> rods;
	TArray<FVector> rivers;
	TArray<FVector> riverNorms;
	TArray<ARiverActor*> riverArray;
	TArray<ARodActor*> rodArray;
	TArray<AFlowerActor*> flowerArray;
	TArray<FString>nodeIDs;
	TArray<FString> rodRiverConnection;
	TSubclassOf<class ARodActor> RodActor;
	TSubclassOf<class AFlowerActor> FlowerActor;
	float offset, scale, zOffset, zScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
		TSubclassOf<class UUserWidget> wTransition;

	ULevelStartWidget* levelStart;

	UMaterial* terrainMaterial;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh)
		UProceduralMeshComponent* terrainMesh;

	virtual void calculateScale();
	FVector transformCoord(FVector coord);

	virtual void createTerrain();
    virtual void addRods();
	virtual void setRodLocations();
	virtual void addRivers();
	virtual void addFlowers();
	virtual void assignConnectionActors();
	virtual void setFlows();
	virtual void computeGoal();

};