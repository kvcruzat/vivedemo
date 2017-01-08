// Fill out your copyright notice in the Description page of Project Settings.

#include "RiversofHanoi.h"
#include "FlowerActor.h"


// Sets default values
AFlowerActor::AFlowerActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));	// scene component to be used as root component

	flowerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlowerMesh"));	// create flower mesh
	flowerMesh->SetupAttachment(SceneComponent);
	RootComponent = SceneComponent;

	ConstructorHelpers::FObjectFinder<UMaterial> redMaterial(TEXT("Material'/Game/models/_Maroon_._Maroon_'"));
	if (redMaterial.Object != NULL) {					// find red material for flower petals
		redRoseMat = (UMaterial*)redMaterial.Object;
	}

	ConstructorHelpers::FObjectFinder<UMaterial> yellowMaterial(TEXT("Material'/Game/models/rose_red1.rose_red1'"));
	if (yellowMaterial.Object != NULL) {				// find yellow material for flower petals
		yellowRoseMat = (UMaterial*)yellowMaterial.Object;
	}

	ConstructorHelpers::FObjectFinder<UMaterialInstance> blueMaterial(TEXT("Material'/Game/VirtualReality/Materials/MI_SmallCubes.MI_SmallCubes'"));
	if (blueMaterial.Object != NULL) {					// find blue material for too much water
		blueRoseMat = (UMaterial*)blueMaterial.Object;
	}

	ConstructorHelpers::FObjectFinder<UMaterial> deadMaterial(TEXT("Material'/Game/models/skp_front_default.skp_front_default'"));
	if (deadMaterial.Object != NULL) {					// find white material for too little water
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

