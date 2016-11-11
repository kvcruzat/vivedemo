// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "RodActor.generated.h"

UCLASS()
class RIVERSOFHANOI_API ARodActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARodActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh)
		UStaticMeshComponent* rodMesh;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	
	
};
