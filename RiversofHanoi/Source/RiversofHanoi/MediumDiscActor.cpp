// Fill out your copyright notice in the Description page of Project Settings.

#include "RiversofHanoi.h"
#include "MediumDiscActor.h"


// Sets default values
AMediumDiscActor::AMediumDiscActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	mediumDiscMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MediumDiscMesh"));
}

// Called when the game starts or when spawned
void AMediumDiscActor::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AMediumDiscActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

