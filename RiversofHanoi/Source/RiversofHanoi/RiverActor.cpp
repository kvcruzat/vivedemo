// Fill out your copyright notice in the Description page of Project Settings.

#include "RiversofHanoi.h"
#include "RiverActor.h"


// Sets default values
ARiverActor::ARiverActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	riverMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("RiverMesh"));

	static ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("Material'/Game/StarterContent/Materials/M_Water_Lake.M_Water_Lake'"));

	if (Material.Object != NULL) {
		riverMaterial = (UMaterial*)Material.Object;
	}
}

// Called when the game starts or when spawned
void ARiverActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARiverActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ARiverActor::createMesh(TArray<FVector> vertexData, TArray<FVector> normals) {
	TArray<int32> triangles;
	triangles.Add(2); //0
	triangles.Add(1); //1
	triangles.Add(0); //2
	triangles.Add(0); //2
	triangles.Add(3); //3
	triangles.Add(2); //0

	riverMesh->CreateMeshSection(0, vertexData, triangles, normals, TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), true);

	riverMesh->SetMaterial(0, riverMaterial);
}

