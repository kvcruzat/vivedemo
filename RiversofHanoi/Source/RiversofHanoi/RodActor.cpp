// Fill out your copyright notice in the Description page of Project Settings.

#include "RiversofHanoi.h"
#include "RodActor.h"

#include "SmallDiscActor.h"
#include "MediumDiscActor.h"
#include "LargeDiscActor.h"

// Sets default values
ARodActor::ARodActor(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    UBoxComponent* BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("RootComponent"));
    RootComponent = BoxComponent;

	collisionComp = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("ColComp"));
	collisionComp->OnComponentHit.AddDynamic(this, &ARodActor::OnHit);

	collisionComp2 = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("ColComp2"));
	collisionComp2->OnComponentBeginOverlap.AddDynamic(this, &ARodActor::OnOverlapBegin);

	rodMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RodMesh"));

	collisionComp->SetupAttachment(rodMesh);
	collisionComp2->SetupAttachment(rodMesh);

}

// Called when the game starts or when spawned
void ARodActor::BeginPlay()
{
	Super::BeginPlay();
	
	rodLocation = this->GetActorLocation();
	rodLocation.Z += 60.0f;

	discRot.Pitch = 0.f;
	discRot.Roll = 0.f;
	
}

// Called every frame
void ARodActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

bool ARodActor::sizeCheck(AActor* actor, TArray<AActor*> actorArray) {

	FString actorName = actor->GetFName().ToString();;
	FString topActorName;

	if (actorArray.Num() > 0) { topActorName = actorArray.Top()->GetFName().ToString();	}
	if (actorArray.Num() == 0) { return true; }
	else if (actorArray.Contains(actor)) { return true; }
	else if (actorName.Contains(TEXT("Medium")) && topActorName.Contains(TEXT("Small"))) { return true;	}
	else if (actorName.Contains(TEXT("Large")) && (topActorName.Contains(TEXT("Medium")) || topActorName.Contains(TEXT("Small"))) ){ return true; }
	else { return false; }
}

void ARodActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	FString collidedActor = OtherActor->GetFName().ToString();
	FString collidedComp = OverlappedComp->GetFName().ToString();

	if (!collidedActor.Contains(TEXT("BP_MotionController")) && !collidedActor.Contains(TEXT("BP_Rod"))) {
		if (collidedComp == TEXT("ColComp2") && containedActors.Contains(OtherActor)) {
			containedActors.Remove(OtherActor);

			if (containedActors.Num() > 0) {
				FString topActorName = containedActors.Top()->GetFName().ToString();
				if (topActorName.Contains(TEXT("Small"))) {
					ASmallDiscActor *actor = Cast<ASmallDiscActor>(containedActors.Top());
					actor->canPickUp = true;

				}
				else if (topActorName.Contains(TEXT("Medium"))) {
					AMediumDiscActor* actor = Cast<AMediumDiscActor>(containedActors.Top());
					actor->canPickUp = true;
				}
			}
		}
		else if (collidedComp == TEXT("ColComp")) {
			if (sizeCheck(OtherActor, containedActors)) {
				if (!containedActors.Contains(OtherActor)) { 
					bool isPickingUp;
					if (collidedActor.Contains(TEXT("Small"))) {
						ASmallDiscActor *actor = Cast<ASmallDiscActor>(OtherActor);
						isPickingUp = actor->isPickingUp;

					}
					else if (collidedActor.Contains(TEXT("Medium"))) {
						AMediumDiscActor* actor = Cast<AMediumDiscActor>(OtherActor);
						isPickingUp = actor->isPickingUp;
					}
					else if (collidedActor.Contains(TEXT("Large"))) {
						ALargeDiscActor* actor = Cast<ALargeDiscActor>(OtherActor);
						isPickingUp = actor->isPickingUp;
					}
					if (!isPickingUp) {
						OtherActor->SetActorRotation(discRot);
						OtherActor->SetActorLocation(rodLocation, false);
					}
					
					containedActors.Add(OtherActor);

					if (containedActors.Num() > 1) {
						for (int32 i = 0; i < containedActors.Num() - 1; ++i) {
							FString currentActorName = containedActors[i]->GetFName().ToString();
							if (currentActorName.Contains(TEXT("Small"))) {
								ASmallDiscActor* actor = Cast<ASmallDiscActor>(containedActors[i]);
								actor->canPickUp = false;
							}
							else if (currentActorName.Contains(TEXT("Medium"))) {
								AMediumDiscActor* actor = Cast<AMediumDiscActor>(containedActors[i]);
								actor->canPickUp = false;
							}


						}
					}
				}

				OverlappedComp->SetCollisionProfileName(FName(TEXT("OverlapAll")));
			}
			else {
				OverlappedComp->SetCollisionProfileName(FName(TEXT("BlockAll")));
			}
		}
	}

	
}

void ARodActor::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	FString collidedActor = OtherActor->GetFName().ToString();
	FString collidedComp = HitComp->GetFName().ToString();

	if (!collidedActor.Contains(TEXT("BP_MotionController")) && !collidedActor.Contains(TEXT("BP_Rod"))) {
		if (collidedComp == TEXT("ColComp")   ){
			if (sizeCheck(OtherActor, containedActors)) {
				if (!containedActors.Contains(OtherActor)) { 
					bool isPickingUp;
					if (collidedActor.Contains(TEXT("Small"))) {
						ASmallDiscActor *actor = Cast<ASmallDiscActor>(OtherActor);
						isPickingUp = actor->isPickingUp;
					}
					else if (collidedActor.Contains(TEXT("Medium"))) {
						AMediumDiscActor* actor = Cast<AMediumDiscActor>(OtherActor);
						isPickingUp = actor->isPickingUp;
					}
					else if (collidedActor.Contains(TEXT("Large"))) {
						ALargeDiscActor* actor = Cast<ALargeDiscActor>(OtherActor);
						isPickingUp = actor->isPickingUp;
					}
					if (!isPickingUp) {
						OtherActor->SetActorRotation(discRot);
						OtherActor->SetActorLocation(rodLocation, false);
					}
					containedActors.Add(OtherActor);

					if (containedActors.Num() > 1) {
						for (int32 i = 0; i < containedActors.Num() - 1; ++i) {
							FString currentActorName = containedActors[i]->GetFName().ToString();
							if (currentActorName.Contains(TEXT("Small"))) {
								ASmallDiscActor* actor = Cast<ASmallDiscActor>(containedActors[i]);
								actor->canPickUp = false;
							}
							else if (currentActorName.Contains(TEXT("Medium"))) {
								AMediumDiscActor* actor = Cast<AMediumDiscActor>(containedActors[i]);
								actor->canPickUp = false;
							}

							
						}
					}
					
				}
				
				HitComp->SetCollisionProfileName(FName(TEXT("OverlapAll")));

			}
			else {
				HitComp->SetCollisionProfileName(FName(TEXT("BlockAll")));
			}
		}
	}


}