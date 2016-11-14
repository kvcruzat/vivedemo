// Fill out your copyright notice in the Description page of Project Settings.

#include "RiversofHanoi.h"
#include "RodActor.h"


// Sets default values
ARodActor::ARodActor(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	collisionComp = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("ColComp"));
	collisionComp->OnComponentBeginOverlap.AddDynamic(this, &ARodActor::OnOverlapBegin);

	rodMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RodMesh"));

	collisionComp->SetupAttachment(rodMesh);

}

// Called when the game starts or when spawned
void ARodActor::BeginPlay()
{
	Super::BeginPlay();
	
	rodLocation = this->GetActorLocation();
	UE_LOG(LogTemp, Warning, TEXT("Location: %s"), *rodLocation.ToString());
	rodLocation.Z += 70.0f;
	
}

// Called every frame
void ARodActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ARodActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	FString collidedActor = OtherActor->GetFName().ToString();

	if (!collidedActor.Contains(TEXT("BP_MotionController")) && !containedActors.Contains(OtherActor)) {
		/*UE_LOG(LogTemp, Warning, TEXT("HIT %s"), *OtherActor->GetFName().ToString());
		UE_LOG(LogTemp, Warning, TEXT("HIT %s"), *OtherComponent->GetFName().ToString());*/
		OtherActor->SetActorLocation(rodLocation, false);

		if (containedActors.Num() != 0) {
			for (int i = 0; i < containedActors.Num(); ++i) {
				UE_LOG(LogTemp, Warning, TEXT("contained: %s"), *containedActors[i]->GetFName().ToString());
			}
		}

		containedActors.Add(OtherActor);

	}
	


}