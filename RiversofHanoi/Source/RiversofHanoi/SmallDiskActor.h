// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SmallDiskActor.generated.h"

UCLASS()
class RIVERSOFHANOI_API ASmallDiskActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASmallDiskActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	TArray<FVector> vertices;
	TArray<FVector> normals;
	TArray<int32> Triangles;
	
	
};
