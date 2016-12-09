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

	collisionComp = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("ColComp"));
	collisionComp->OnComponentHit.AddDynamic(this, &ARodActor::OnHit);

	collisionComp2 = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("ColComp2"));
	collisionComp2->OnComponentBeginOverlap.AddDynamic(this, &ARodActor::OnOverlapBegin);
	collisionComp2->OnComponentEndOverlap.AddDynamic(this, &ARodActor::OnOverlapEnd);

	rodMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RodMesh"));

	collisionComp->SetupAttachment(rodMesh);
	collisionComp2->SetupAttachment(rodMesh);

}

// Called when the game starts or when spawned
void ARodActor::BeginPlay()
{
	Super::BeginPlay();
	
	rodLocation = this->GetActorLocation();
	rodLocation.Z += 40.0f;

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

	if (!collidedActor.Contains(TEXT("BP_MotionController")) && !collidedActor.Contains(TEXT("BP_Rod")) && !collidedActor.Contains(TEXT("TerrainActor")) ) {
		if (collidedComp == TEXT("ColComp2")) {
			if (sizeCheck(OtherActor, containedActors)){
				UE_LOG(LogTemp, Warning, TEXT("#%s CAN BE PLACED"), *collidedActor);
				collisionComp->SetCollisionProfileName(FName(TEXT("OverlapAllDynamic")));
				if (!containedActors.Contains(OtherActor)) {
					containedActors.Add(OtherActor);
					arrayChange(true, collidedActor);

					UE_LOG(LogTemp, Warning, TEXT("#%s containedNUM: %s"), *FString::FromInt(FMath::RandRange(1, 10)), *FString::FromInt(containedActors.Num()));
					for (int32 i = 0; i < containedActors.Num(); ++i) {
						UE_LOG(LogTemp, Warning, TEXT("# %s"), *containedActors[i]->GetFName().ToString());
					}

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
				UE_LOG(LogTemp, Warning, TEXT("# %s ERROR CONTAINS"), *collidedActor);
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("# %s CANT BE PLACED. TELEPORT%s"), *collidedActor);
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
					rodLocation.X += 40.0f;
					FHitResult HitResult;
					OtherActor->SetActorLocation(rodLocation, false, &HitResult, ETeleportType::None);
					rodLocation.X -= 40.0f;
				}
				
			}
		}
		else if (collidedComp == TEXT("ColComp")) {
			if (sizeCheck(OtherActor, containedActors)) {
				OverlappedComp->SetCollisionProfileName(FName(TEXT("OverlapAllDynamic")));
			}
			else {
				OverlappedComp->SetCollisionProfileName(FName(TEXT("BlockAll")));
			}
		}
	}
	
	
}

void ARodActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex) {
	FString collidedActor = OtherActor->GetFName().ToString();
	FString collidedComp = OverlappedComp->GetFName().ToString();

	if (!collidedActor.Contains(TEXT("BP_MotionController")) && !collidedActor.Contains(TEXT("BP_Rod")) && !collidedActor.Contains(TEXT("TerrainActor")) ){
		if (collidedComp == TEXT("ColComp2") && containedActors.Contains(OtherActor)) {
			containedActors.Pop(true);
			arrayChange(false, collidedActor);
			collisionComp->SetCollisionProfileName(FName(TEXT("BlockAll")));
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
			UE_LOG(LogTemp, Warning, TEXT("#%s containedNUM: %s"), *FString::FromInt(FMath::RandRange(1, 10)), *FString::FromInt(containedActors.Num()));
			UE_LOG(LogTemp, Warning, TEXT("# contained: "));
			for (int32 i = 0; i < containedActors.Num(); ++i) {
				UE_LOG(LogTemp, Warning, TEXT("# %s"), *containedActors[i]->GetFName().ToString());
			}
		}
		else if (collidedComp == TEXT("ColComp")) {
			OverlappedComp->SetCollisionProfileName(FName(TEXT("BlockAll")));
		}
	}
	
}

void ARodActor::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	FString collidedActor = OtherActor->GetFName().ToString();
	FString collidedComp = HitComp->GetFName().ToString();

	if (!collidedActor.Contains(TEXT("BP_MotionController")) && !collidedActor.Contains(TEXT("BP_Rod")) && !collidedActor.Contains(TEXT("TerrainActor"))) {
		if (collidedComp == TEXT("ColComp")   ){
			if (sizeCheck(OtherActor, containedActors)) {
				HitComp->SetCollisionProfileName(FName(TEXT("OverlapAllDynamic")));
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
						FHitResult HitResult;
						OtherActor->SetActorLocation(rodLocation, false, &HitResult, ETeleportType::None);
					}
				}
			}
			else {
				HitComp->SetCollisionProfileName(FName(TEXT("BlockAll")));
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
					if (collidedActor.Contains(TEXT("Small"))) {
						ASmallDiscActor *actor = Cast<ASmallDiscActor>(OtherActor);
						actor->smallDiscMesh->AddForce(FVector(0, 0, 20000));
					}
					else if (collidedActor.Contains(TEXT("Medium"))) {
						AMediumDiscActor* actor = Cast<AMediumDiscActor>(OtherActor);
						actor->mediumDiscMesh->AddForce(FVector(0, 0, 20000));
					}
					else if (collidedActor.Contains(TEXT("Large"))) {
						ALargeDiscActor* actor = Cast<ALargeDiscActor>(OtherActor);
						actor->largeDiscMesh->AddForce(FVector(0, 0, 20000));
					}
					/*OtherActor->SetActorRotation(discRot);
					rodLocation.X += 40.0f;
					FHitResult HitResult;
					OtherActor->SetActorLocation(rodLocation, false, &HitResult, ETeleportType::None);
					rodLocation.X -= 40.0f;*/
				}
			}
		}
	}

}

void ARodActor::arrayChange(bool add, FString discName) {
	int32 containedSize = containedActors.Num();
	
	float incomingFlow = 0;

	if (nodeID.Contains(TEXT("00"))) {
		incomingFlow += 24;
	}
	else {
		for (int riverIndex = 0; riverIndex < inputRivers.Num(); riverIndex++) {
			incomingFlow += inputRivers[riverIndex]->flow;
		}
	}

	incomingFlow = incomingFlow / 2;


	float flowDiff = 0;
	
	if (add) {
		if (discName.Contains(TEXT("Small"))) { 
			connectedRiver->discStatus += (-(1.0f / 6.0f));
			flowDiff = (incomingFlow + (incomingFlow * connectedRiver->discStatus)) - connectedRiver->flow;
			connectedRiver->changeFlow(flowDiff);

/*			OtherRod->connectedRiver->discStatus += (1.0f / 6.0f);
			flowDiff = (incomingFlow + (incomingFlow * OtherRod->connectedRiver->discStatus)) - OtherRod->connectedRiver->flow;
			OtherRod->connectedRiver->changeFlow(flowDiff);		*/	
		}
		else if (discName.Contains(TEXT("Medium"))) {
			connectedRiver->discStatus += (-(1.0f / 3.0f));
			flowDiff = (incomingFlow + (incomingFlow * connectedRiver->discStatus)) - connectedRiver->flow;
			connectedRiver->changeFlow(flowDiff);

			//OtherRod->connectedRiver->discStatus += (1.0f / 3.0f);
			//flowDiff = (incomingFlow + (incomingFlow * OtherRod->connectedRiver->discStatus)) - OtherRod->connectedRiver->flow;
			//OtherRod->connectedRiver->changeFlow(flowDiff);
		}
		else if (discName.Contains(TEXT("Large"))) {
			if (containedSize == 3) {
				connectedRiver->discStatus = -1;
				connectedRiver->changeFlow(-(connectedRiver->flow));
			}
			else {
				connectedRiver->discStatus += (-(1.0f / 2.0f));
				flowDiff = (incomingFlow + (incomingFlow * connectedRiver->discStatus)) - connectedRiver->flow;
				connectedRiver->changeFlow(flowDiff);
			}

			//OtherRod->connectedRiver->discStatus += (1.0f / 2.0f);
			//flowDiff = (incomingFlow + (incomingFlow * OtherRod->connectedRiver->discStatus)) - OtherRod->connectedRiver->flow;
			//OtherRod->connectedRiver->changeFlow(flowDiff);
		}
	}
	else {
		if (discName.Contains(TEXT("Small"))) {
				connectedRiver->discStatus = 0;
				flowDiff = (incomingFlow + (incomingFlow * connectedRiver->discStatus)) - connectedRiver->flow;
				connectedRiver->changeFlow(flowDiff);

			//OtherRod->connectedRiver->discStatus += (-1.0f / 6.0f);
			//flowDiff = (incomingFlow + (incomingFlow * OtherRod->connectedRiver->discStatus)) - OtherRod->connectedRiver->flow;
			//OtherRod->connectedRiver->changeFlow(flowDiff);
		}
		else if (discName.Contains(TEXT("Medium"))) {
			connectedRiver->discStatus += ((1.0f / 3.0f));
			flowDiff = (incomingFlow + (incomingFlow * connectedRiver->discStatus)) - connectedRiver->flow;
			connectedRiver->changeFlow(flowDiff);

			//OtherRod->connectedRiver->discStatus += (-1.0f / 3.0f);
			//flowDiff = (incomingFlow + (incomingFlow * OtherRod->connectedRiver->discStatus)) - OtherRod->connectedRiver->flow;
			//OtherRod->connectedRiver->changeFlow(flowDiff);
		}
		else if (discName.Contains(TEXT("Large"))) {
			connectedRiver->discStatus += ((1.0f / 2.0f));
			flowDiff = (incomingFlow + (incomingFlow * connectedRiver->discStatus)) - connectedRiver->flow;
			connectedRiver->changeFlow(flowDiff);

			//OtherRod->connectedRiver->discStatus += (-1.0f / 2.0f);
			//flowDiff = (incomingFlow + (incomingFlow * OtherRod->connectedRiver->discStatus)) - OtherRod->connectedRiver->flow;
			//OtherRod->connectedRiver->changeFlow(flowDiff);
		}
	}
}