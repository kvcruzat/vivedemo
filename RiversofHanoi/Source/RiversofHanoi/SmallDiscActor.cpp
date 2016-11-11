// Fill out your copyright notice in the Description page of Project Settings.

#include "RiversofHanoi.h"
#include "SmallDiscActor.h"


// Sets default values
ASmallDiscActor::ASmallDiscActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	smallDiscMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SmallDiscMesh"));

}

// Called when the game starts or when spawned
void ASmallDiscActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASmallDiscActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

