// Fill out your copyright notice in the Description page of Project Settings.

#include "RiversofHanoi.h"
#include "LargeDiscActor.h"


// Sets default values
ALargeDiscActor::ALargeDiscActor(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    largeDiscMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LargeDiscMesh"));	// large disc mesh

    collisionComp = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("DiscColComp")); // create and attach a collision box
    collisionComp->OnComponentBeginOverlap.AddDynamic(this, &ALargeDiscActor::OnOverlapBegin);

    collisionComp->SetupAttachment(largeDiscMesh);

    RootComponent = largeDiscMesh;
}

// Called when the game starts or when spawned
void ALargeDiscActor::BeginPlay()
{
	Super::BeginPlay();

    teleportLocation = this->GetActorLocation();	// use original disc location
}

// Called every frame
void ALargeDiscActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

// collision box overlap trigger
void ALargeDiscActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
    FString collidedActor = OtherActor->GetFName().ToString();
    FString collidedComp = OtherComponent->GetFName().ToString();
    UE_LOG(LogTemp, Warning, TEXT("#%s HIT"), *collidedActor);

    if (collidedActor.Contains((TEXT("TriggerVolume"))))	// teleport to original disc location when overlaps with box volume underneath the terrain
    {
        FHitResult HitResult;
        this->SetActorLocation(teleportLocation, false, &HitResult, ETeleportType::None);
    }
}
