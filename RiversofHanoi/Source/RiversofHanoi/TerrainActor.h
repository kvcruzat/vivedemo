// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
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

	virtual void readData(FString fileName);

	virtual void findNormals();
	virtual void normalise();

	TArray<FVector> vertices;
	TArray<FVector> normals;
	TArray<int32> Triangles;

};