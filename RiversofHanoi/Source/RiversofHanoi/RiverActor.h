// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "RiverActor.generated.h"

UCLASS()
class RIVERSOFHANOI_API ARiverActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARiverActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	TArray<FVector> vertices;
	TArray<FVector> normals;
	TArray<int32> Triangles;
	UMaterial* riverMaterial;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh)
		UProceduralMeshComponent* riverMesh;
	
	virtual void ARiverActor::createMesh(TArray<FVector> vertexData, TArray<FVector> normals);
};
