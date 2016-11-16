// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SmallDiscActor.generated.h"

UCLASS()
class RIVERSOFHANOI_API ASmallDiscActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASmallDiscActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh)
	UStaticMeshComponent* smallDiscMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool canPickUp = true;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	
	
};
