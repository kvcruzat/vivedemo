// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "RiverActor.h"
#include "RodActor.generated.h"

UCLASS()
class RIVERSOFHANOI_API ARodActor : public AActor
{
	GENERATED_UCLASS_BODY()
	
public:	
	// Sets default values for this actor's properties
	//ARodActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = collision, meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* collisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = collision, meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* collisionComp2;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh)
		class UStaticMeshComponent* rodMesh;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Method to check if the disc that is about to be placed on the rod is allowed to be
	virtual bool sizeCheck(AActor* actor, TArray<AActor*> actorArray);

	// Array of actors that is on the rod
	TArray<AActor*> containedActors;

	UPROPERTY(EditAnywhere)
	ARiverActor* connectedRiver;	// the river the rod outputs to

	UPROPERTY(EditAnywhere)
	TArray<ARiverActor*> inputRivers;	// rivers that is used as input

	UPROPERTY(EditAnywhere)
	ARodActor* OtherRod;	// the other rod in the node

	FVector rodLocation;	// the location of the rod in the world
	FRotator discRot;		// original disc rotation
	FString rodID;

	UPROPERTY(EditAnywhere)
	FString nodeID;		// node ID the rod belongs to

	// Method that is called when the a disc is added or removed to change the flows that is affected by the change
	virtual void arrayChange(bool add, FString discName);
	
	FString riverConnection;	// connected river ID

	// Different collision box triggers
	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex);
	
};
