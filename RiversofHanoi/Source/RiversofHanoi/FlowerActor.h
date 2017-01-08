// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "FlowerActor.generated.h"

UCLASS()
class RIVERSOFHANOI_API AFlowerActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFlowerActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh)
	class UStaticMeshComponent* flowerMesh;		// flower mesh

	UPROPERTY(EditAnywhere)
	float requiredFlow;	// flow required to make flower alive

	UPROPERTY(EditAnywhere)
	FString nodeID;

	// materials used for flower petals
	UMaterial* redRoseMat;
	UMaterial* yellowRoseMat;
	UMaterial* blueRoseMat;
	UMaterial* deadRoseMat;

	
};
