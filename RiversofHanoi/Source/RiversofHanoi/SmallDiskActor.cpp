// Fill out your copyright notice in the Description page of Project Settings.

#include "RiversofHanoi.h"
#include "SmallDiskActor.h"
#include "Util.h"

#include "ProceduralMeshComponent.h"

// Sets default values
ASmallDiskActor::ASmallDiskActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Util *util = new Util();;
	util->readData("torus.m", vertices, normals, Triangles);
	vertices = util->verts;
	normals = util->norms;
	Triangles = util->triangs;

	USphereComponent* SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	RootComponent = SphereComponent;

	UProceduralMeshComponent* smallDiskMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("SmallDiskMesh"));

	smallDiskMesh->CreateMeshSection(0, vertices, Triangles, normals, TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), true);
	smallDiskMesh->SetMobility(EComponentMobility::Movable);
	smallDiskMesh->SetSimulatePhysics(true);
	smallDiskMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	smallDiskMesh->SetupAttachment(RootComponent);
	SetActorEnableCollision(true);


}

// Called when the game starts or when spawned
void ASmallDiskActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASmallDiskActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

