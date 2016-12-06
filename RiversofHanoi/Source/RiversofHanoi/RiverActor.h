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

	UPROPERTY(EditAnywhere)
	UMaterial* riverMaterial;

	UPROPERTY(EditAnywhere)
	UMaterialInstanceDynamic* riverMaterialInstance;

	UPROPERTY(EditAnywhere)
	float flow;

	UPROPERTY(EditAnywhere)
	FString riverID;

	UPROPERTY(EditAnywhere)
	FString nodeID;

	UPROPERTY(EditAnywhere)
	FString outputNode;

	UPROPERTY(EditAnywhere)
	TArray<ARiverActor*> overlappedRivers;
	UPROPERTY(EditAnywhere)
	TArray<ARiverActor*> outputRivers;

	TArray<FString> riverConnections;

	UPROPERTY(EditAnywhere)
	TArray<ARiverActor*> connectedRivers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
		UProceduralMeshComponent* riverMesh;
	
	virtual void createMesh(TArray<FVector> vertexData);
	virtual void changeFlow(float value);
	/*virtual void computeFlows();*/
};
