// Fill out your copyright notice in the Description page of Project Settings.

#include "RiversofHanoi.h"
#include "FlowerActor.h"


// Sets default values
AFlowerActor::AFlowerActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));

	flowerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlowerMesh"));

	flowerMesh->SetupAttachment(SceneComponent);

	RootComponent = SceneComponent;

}

// Called when the game starts or when spawned
void AFlowerActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFlowerActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

