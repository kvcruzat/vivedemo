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

	ConstructorHelpers::FObjectFinder<UMaterial> redMaterial(TEXT("Material'/Game/models/_Maroon_._Maroon_'"));

	if (redMaterial.Object != NULL) {
		redRoseMat = (UMaterial*)redMaterial.Object;
	}

	ConstructorHelpers::FObjectFinder<UMaterial> yellowMaterial(TEXT("Material'/Game/models/rose_red1.rose_red1'"));

	if (yellowMaterial.Object != NULL) {
		yellowRoseMat = (UMaterial*)yellowMaterial.Object;
	}

	ConstructorHelpers::FObjectFinder<UMaterialInstance> blueMaterial(TEXT("Material'/Game/VirtualReality/Materials/MI_SmallCubes.MI_SmallCubes'"));

	if (blueMaterial.Object != NULL) {
		blueRoseMat = (UMaterial*)blueMaterial.Object;
	}

	ConstructorHelpers::FObjectFinder<UMaterial> deadMaterial(TEXT("Material'/Game/models/skp_front_default.skp_front_default'"));

	if (deadMaterial.Object != NULL) {
		deadRoseMat = (UMaterial*)deadMaterial.Object;
	}
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

