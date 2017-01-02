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
	// PrimaryActorTick.bCanEverTick = true;

	// create and attach collision boxes
	collisionComp = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("ColComp"));
	collisionComp->OnComponentHit.AddDynamic(this, &ARodActor::OnHit);

	collisionComp2 = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("ColComp2"));
	collisionComp2->OnComponentBeginOverlap.AddDynamic(this, &ARodActor::OnOverlapBegin);
	collisionComp2->OnComponentEndOverlap.AddDynamic(this, &ARodActor::OnOverlapEnd);

	rodMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RodMesh"));	// create rod mesh

	collisionComp->SetupAttachment(rodMesh);
	collisionComp2->SetupAttachment(rodMesh);

}

// Called when the game starts or when spawned
void ARodActor::BeginPlay()
{
	Super::BeginPlay();
	
	rodLocation = this->GetActorLocation();
	rodLocation.Z += 40.0f;			// use the top of the rod as the location for the disc snap to rod feature

	discRot.Pitch = 0.f;	// used for reseting the disc rotation when snapped to rod
	discRot.Roll = 0.f;
	
}

// Called every frame
void ARodActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

// Method to check if the disc to be placed can be placed according to Towers of Hanoi logic
bool ARodActor::sizeCheck(AActor* actor, TArray<AActor*> actorArray) {

	FString actorName = actor->GetFName().ToString();;
	FString topActorName;

	if (actorArray.Num() > 0) { topActorName = actorArray.Top()->GetFName().ToString();	}
	if (actorArray.Num() == 0) { return true; }	// if rod is empty then allow
	else if (actorArray.Contains(actor)) { return true; }	// disc already in the array
	else if (actorName.Contains(TEXT("Medium")) && topActorName.Contains(TEXT("Small"))) { return true;	}	// medium being placed on top of small
	else if (actorName.Contains(TEXT("Large")) && (topActorName.Contains(TEXT("Medium")) || topActorName.Contains(TEXT("Small"))) ){ return true; } // large being place on top of medium
	else { return false; }	// dont allow disc to be placed
}

// Method that handles collision triggers on start of overlap begin
void ARodActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	FString collidedActor = OtherActor->GetFName().ToString();
	FString collidedComp = OverlappedComp->GetFName().ToString();
	// dismisses collisions with player hand models, the rod itself and terrain
	if (!collidedActor.Contains(TEXT("BP_MotionController")) && !collidedActor.Contains(TEXT("BP_Rod")) && !collidedActor.Contains(TEXT("TerrainActor")) ) {
		if (collidedComp == TEXT("ColComp2")) {
			if (sizeCheck(OtherActor, containedActors)){	// disc placement check
				collisionComp->SetCollisionProfileName(FName(TEXT("OverlapAllDynamic")));	// allow disc to go through the box
				if (!containedActors.Contains(OtherActor)) {	// if disc is not already in the array
					containedActors.Add(OtherActor);	// add to array of contained discs
					arrayChange(true, collidedActor);	// change all affected flows

					UE_LOG(LogTemp, Warning, TEXT("#%s containedNUM: %s"), *FString::FromInt(FMath::RandRange(1, 10)), *FString::FromInt(containedActors.Num()));
					for (int32 i = 0; i < containedActors.Num(); ++i) {
						UE_LOG(LogTemp, Warning, TEXT("# %s"), *containedActors[i]->GetFName().ToString());
					}

					// make all discs below the newly placed disc not able to be picked up
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
			}
			else {
				// check if disc is being currently being held. If not then move the disc away from the rod
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
			if (sizeCheck(OtherActor, containedActors)) { // set collision to allow disc if sizeCheck is true else block it
				OverlappedComp->SetCollisionProfileName(FName(TEXT("OverlapAllDynamic")));
			}
			else {
				OverlappedComp->SetCollisionProfileName(FName(TEXT("BlockAll")));
			}
		}
	}
	
	
}

// Method handles collision triggers on overlap end
void ARodActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex) {
	FString collidedActor = OtherActor->GetFName().ToString();
	FString collidedComp = OverlappedComp->GetFName().ToString();

	// dismisses collisions with player hand models, the rod itself and terrain
	if (!collidedActor.Contains(TEXT("BP_MotionController")) && !collidedActor.Contains(TEXT("BP_Rod")) && !collidedActor.Contains(TEXT("TerrainActor")) ){
		if (collidedComp == TEXT("ColComp2") && containedActors.Contains(OtherActor)) {	// when disc leaves collision box, it is removed
			containedActors.Pop(true);	// remove disc
			arrayChange(false, collidedActor);	// change affecting flows
			collisionComp->SetCollisionProfileName(FName(TEXT("BlockAll"))); // reset collision box to block

			// changes the new top disc to be allowed to be picked up
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
		else if (collidedComp == TEXT("ColComp")) {	// makes sure collision box is set to block all
			OverlappedComp->SetCollisionProfileName(FName(TEXT("BlockAll")));
		}
	}
	
}

// Method that is triggered when collision box is blocking is hit
void ARodActor::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	FString collidedActor = OtherActor->GetFName().ToString();
	FString collidedComp = HitComp->GetFName().ToString();

	// dismisses collisions with player hand models, the rod itself and terrain
	if (!collidedActor.Contains(TEXT("BP_MotionController")) && !collidedActor.Contains(TEXT("BP_Rod")) && !collidedActor.Contains(TEXT("TerrainActor"))) {
		if (collidedComp == TEXT("ColComp")   ){
			if (sizeCheck(OtherActor, containedActors)) {	// checks if disc is allowed to be placed
				HitComp->SetCollisionProfileName(FName(TEXT("OverlapAllDynamic")));
				if (!containedActors.Contains(OtherActor)) {	// makes sure disc is not already in
					// check if player is holding disc
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
						OtherActor->SetActorRotation(discRot);	// if player is not holding this then use snap to rod feature
						FHitResult HitResult;
						OtherActor->SetActorLocation(rodLocation, false, &HitResult, ETeleportType::None);
					}
				}
			}
			else {	// disc is not allowed to be placed
				HitComp->SetCollisionProfileName(FName(TEXT("BlockAll")));

				// check if player is holding the disc
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
				if (!isPickingUp) {	// add an upwards force to the disc to prevent it being placed
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

// When disc is added or removed this method is called to change the affecting flows
void ARodActor::arrayChange(bool add, FString discName) {
	int32 containedSize = containedActors.Num();
	
	float incomingFlow = 0;

	// check if the node is the source node
	if (nodeID.Contains(TEXT("00"))) {
		incomingFlow += 24;	// source node flow
	}
	else {
		for (int riverIndex = 0; riverIndex < inputRivers.Num(); riverIndex++) { // accumulate all the flows from input rivers
			incomingFlow += inputRivers[riverIndex]->flow;
		}
	}

	incomingFlow = incomingFlow / 2;	// split flow in half between the two rods in the node


	float flowDiff = 0;
	
	// check if the disc change was an addition of a disc
	if (add) {
		if (discName.Contains(TEXT("Small"))) {	// small disc added
			connectedRiver->discStatus += (-(1.0f / 6.0f));	// update disc status to subtract 1/6 of flow
			flowDiff = (incomingFlow + (incomingFlow * connectedRiver->discStatus)) - connectedRiver->flow;	// calculate the difference between the current and new flow
			connectedRiver->changeFlow(flowDiff);	// update
	
		}
		else if (discName.Contains(TEXT("Medium"))) {	// medium disc added
			connectedRiver->discStatus += (-(1.0f / 3.0f));	// update disc status to subtract 1/3 of flow
			flowDiff = (incomingFlow + (incomingFlow * connectedRiver->discStatus)) - connectedRiver->flow;
			connectedRiver->changeFlow(flowDiff);

		}
		else if (discName.Contains(TEXT("Large"))) {	// large disc added
			if (containedSize == 3) {	// check if rod is full
				connectedRiver->discStatus = -1;	// updae disc status to completely remove flow
				connectedRiver->changeFlow(-(connectedRiver->flow));
			}
			else {
				connectedRiver->discStatus += (-(1.0f / 2.0f));	// update disc status to subtract 1/2 of flow
				flowDiff = (incomingFlow + (incomingFlow * connectedRiver->discStatus)) - connectedRiver->flow;
				connectedRiver->changeFlow(flowDiff);
			}

		}
	}
	else {	// disc is being removed
		if (discName.Contains(TEXT("Small"))) {	// small disc removed
				connectedRiver->discStatus = 0;	// reset disc status to allow all possible flow
				flowDiff = (incomingFlow + (incomingFlow * connectedRiver->discStatus)) - connectedRiver->flow;
				connectedRiver->changeFlow(flowDiff);

		}
		else if (discName.Contains(TEXT("Medium"))) {	// medium disc removed
			connectedRiver->discStatus += ((1.0f / 3.0f));	// update disc status to reallow 1/3 of flow
			flowDiff = (incomingFlow + (incomingFlow * connectedRiver->discStatus)) - connectedRiver->flow;
			connectedRiver->changeFlow(flowDiff);

		}
		else if (discName.Contains(TEXT("Large"))) {
			connectedRiver->discStatus += ((1.0f / 2.0f));	// large disc removed
			flowDiff = (incomingFlow + (incomingFlow * connectedRiver->discStatus)) - connectedRiver->flow;
			connectedRiver->changeFlow(flowDiff);	// update disc status to reallow 1/2 of flow

		}
	}
}