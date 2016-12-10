// Fill out your copyright notice in the Description page of Project Settings.

#include "RiversofHanoi.h"
#include "MediumDiscActor.h"


// Sets default values
AMediumDiscActor::AMediumDiscActor(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    mediumDiscMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MediumDiscMesh"));
    
    collisionComp = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("ColComp"));
    collisionComp->OnComponentBeginOverlap.AddDynamic(this, &AMediumDiscActor::OnOverlapBegin);

    collisionComp->SetupAttachment(mediumDiscMesh);

    RootComponent = mediumDiscMesh;
}

// Called when the game starts or when spawned
void AMediumDiscActor::BeginPlay()
{
	Super::BeginPlay();

    teleportLocation = this->GetActorLocation();

}

// Called every frame
void AMediumDiscActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AMediumDiscActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
    FString collidedActor = OtherActor->GetFName().ToString();
    FString collidedComp = OverlappedComp->GetFName().ToString();
    UE_LOG(LogTemp, Warning, TEXT("#%s HIT"), *collidedActor);

    if (collidedActor.Contains((TEXT("TriggerVolume"))))
    {
        FHitResult HitResult;
        this->SetActorLocation(teleportLocation, false, &HitResult, ETeleportType::None);
    }
}