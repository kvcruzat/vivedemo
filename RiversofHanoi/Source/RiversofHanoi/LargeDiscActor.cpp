// Fill out your copyright notice in the Description page of Project Settings.

#include "RiversofHanoi.h"
#include "LargeDiscActor.h"


// Sets default values
ALargeDiscActor::ALargeDiscActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	largeDiscMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LargeDiscMesh"));

}

// Called when the game starts or when spawned
void ALargeDiscActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALargeDiscActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

