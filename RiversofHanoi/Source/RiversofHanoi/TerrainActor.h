// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "RodActor.h"
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

	TArray<FVector> vertices;
	TArray<FVector> normals;
	TArray<int32> Triangles;
    TArray<FVector> nodes;
	TArray<ARodActor*> rodArray;
	TSubclassOf<class ARodActor> RodActor;

    virtual void addRods();

	virtual void setRodLocations();

};