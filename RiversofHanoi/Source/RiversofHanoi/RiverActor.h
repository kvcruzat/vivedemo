// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "FlowerActor.h"
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
	
	// arrays used for river mesh creation
	TArray<FVector> vertices;
	TArray<FVector> normals;
	TArray<int32> Triangles;

	bool initialSet;	// to check if it's the first time to set the flow
	float discStatus;	// disc status corresponds to how discs in connected rod

	UPROPERTY(EditAnywhere)
	UMaterial* riverMaterial;	// water material

	UPROPERTY(EditAnywhere)	// instance to change programmatically
	UMaterialInstanceDynamic* riverMaterialInstance;

	UPROPERTY(EditAnywhere)	// current flow
	float flow;

	UPROPERTY(EditAnywhere)
	FString riverID;

	UPROPERTY(EditAnywhere)	// ID of node taking in as input if available
	FString nodeID;

	UPROPERTY(EditAnywhere)	// ID of node output if available
	FString outputNode;

	UPROPERTY(EditAnywhere)	// array of all rivers overlapping for use in the output node if any
	TArray<ARiverActor*> overlappedRivers;
	UPROPERTY(EditAnywhere)	// rivers that the river will be outputting to in the node
	TArray<ARiverActor*> outputRivers;

	TArray<FString> riverConnections;	// array of all river IDs between two nodes this river affects

	UPROPERTY(EditAnywhere)	// array of all river actors between two nodes this river affects
	TArray<ARiverActor*> connectedRivers;

	UPROPERTY(EditAnywhere)	// array of flower actors this river is affecting if any
	TArray<AFlowerActor*> flowerArray;

	UPROPERTY(EditAnywhere)	// the final river actor that outputs to the last node in the level
	ARiverActor* finalNodeRiver;

	UPROPERTY(EditAnywhere)	// used by last river actor to check goal completion
	TArray<bool> nodeGoals;

	UPROPERTY(EditAnywhere)	// array of node IDs which have flowers
	TArray<FString> flowerNodeIDs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)	// river mesh
		UProceduralMeshComponent* riverMesh;
	
	virtual void createMesh(TArray<FVector> vertexData);	// mesh creation
	virtual void changeFlow(float value);	// update flow of current river actor and affecting rivers
	virtual void waterFlowers();	// update the status of flowers
	virtual void checkLevelCompletion();	// check if all flower goals are completed
	virtual void changeLevel();		// transition to next level
};
