// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "LargeDiscActor.generated.h"

UCLASS()
class RIVERSOFHANOI_API ALargeDiscActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALargeDiscActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh)
		UStaticMeshComponent* largeDiscMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool canPickUp = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isPickingUp = false;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	
	
};
