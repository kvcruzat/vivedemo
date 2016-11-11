// Fill out your copyright notice in the Description page of Project Settings.

#include "RiversofHanoi.h"
#include "RodActor.h"


// Sets default values
ARodActor::ARodActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	rodMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RodMesh"));
}

// Called when the game starts or when spawned
void ARodActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARodActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

