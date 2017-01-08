// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SmallDiscActor.generated.h"

UCLASS()
class RIVERSOFHANOI_API ASmallDiscActor : public AActor
{
	GENERATED_UCLASS_BODY()
	
public:	
	// Sets default values for this actor's properties
	//ASmallDiscActor(const FObjectInitializer& ObjectInitializer);

	// collision box
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = collision, meta = (AllowPrivateAccess = "true"))
        class UBoxComponent* collisionComp;

	// mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh)
		class UStaticMeshComponent* smallDiscMesh;

	// bool to say if the disc is allowed to be picked up
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool canPickUp = true;

	// bool to say if the disc is being currently held
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isPickingUp = false;

	// collision trigger on overlap begin
    UFUNCTION()
        void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// coordinate of original disc location to be teleported back to when out of bounds
    FVector teleportLocation;
	
};
