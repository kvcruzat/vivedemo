// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "MediumDiscActor.generated.h"

UCLASS()
class RIVERSOFHANOI_API AMediumDiscActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMediumDiscActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh)
		UStaticMeshComponent* mediumDiscMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool canPickUp = true;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	
	
};
