// Fill out your copyright notice in the Description page of Project Settings.

#include "RiversofHanoi.h"
#include "RiverActor.h"
#include "Util.h"
#include "Blueprint/UserWidget.h"


// Sets default values
ARiverActor::ARiverActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// PrimaryActorTick.bCanEverTick = true;

	// initial values
	flow = 0;
	discStatus = 0;
	initialSet = true;
	outputNode = TEXT("");

	riverMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("RiverMesh"));	// create mesh

	// find the water material
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

// river mesh creation
void ARiverActor::createMesh(TArray<FVector> vertexData) {
	
	// triangle face definition
	TArray<int32> triangles;
	triangles.Add(2); //0
	triangles.Add(1); //1
	triangles.Add(0); //2
	triangles.Add(0); //2
	triangles.Add(3); //3
	triangles.Add(2); //0

	Util *util = new Util();
	normals = util->findNormals(vertexData, triangles);	// compute normals

	// create mesh
	riverMesh->CreateMeshSection(0, vertexData, triangles, normals, TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), false);
	
	riverMesh->SetMaterial(0, riverMaterial);	// set water material
	riverMesh->SetCollisionProfileName(FName(TEXT("OverlapAll")));	// set collision of river to overlap all
	riverMesh->SetCanEverAffectNavigation(false);	// don't allow player navigation to be affected
	riverMaterialInstance = riverMesh->CreateDynamicMaterialInstance(0);	// create dynamic instance
	
}

// change current and affected river flows recursively
void ARiverActor::changeFlow(float value) {

	flow += value;	// change current river flow

	// check if river is ouputting to a node
	if (connectedRivers.Num() == 0) {
		float totalFlow = flow;

		for (int river = 0; river < overlappedRivers.Num(); river++) {
			totalFlow += overlappedRivers[river]->flow;	// accumulate flows coming in to the node
		}

		float incomingFlow = totalFlow / 2;	// split flow

		float flowDiff = 0;

		for (int river = 0; river < outputRivers.Num(); river++) {	// calculate and change flows of the rivers at the two rods depending on disc status
			flowDiff = (incomingFlow + (incomingFlow * outputRivers[river]->discStatus)) - outputRivers[river]->flow;
			outputRivers[river]->changeFlow(flowDiff);
		}

		if (flowerArray.Num() > 0){	// check if there are any flowers dependent on this river
			waterFlowers();		// update flowers status
			checkLevelCompletion();	// check if level is completed
		}

	}
	else {	// river is connected to other rivers that need to be changed
		for (int Index = 0; Index < connectedRivers.Num(); Index++) {	// iterate through all connected rivers

			connectedRivers[Index]->changeFlow(value);	// change flow of connected river

			if (Index == connectedRivers.Num() - 1 && !connectedRivers[Index]->outputNode.IsEmpty()) {	// check if it is the last river in the connection and if it has an output node
				float totalFlow = connectedRivers[Index]->flow;

				for (int river = 0; river < connectedRivers[Index]->overlappedRivers.Num(); river++) {
					totalFlow += connectedRivers[Index]->overlappedRivers[river]->flow;	// accumulate all river flows that output to a node
				}

				float incomingFlow = totalFlow / 2;

				float flowDiff = 0;

				for (int river = 0; river < connectedRivers[Index]->outputRivers.Num(); river++) {	// change flows of output rivers in the node
					flowDiff = (incomingFlow + (incomingFlow * connectedRivers[Index]->outputRivers[river]->discStatus)) - connectedRivers[Index]->outputRivers[river]->flow;
					connectedRivers[Index]->outputRivers[river]->changeFlow(flowDiff);
				}

			}

		}
	}

	float startFlow = 24;	// the starting flow at source node

	// change the opacity of river actor material depending on flow
	if (flow > startFlow * 0.9) {
		riverMaterialInstance->SetScalarParameterValue(TEXT("Opacity"), 0.9);
	}
	else if (flow > startFlow * 0.8) {
		riverMaterialInstance->SetScalarParameterValue(TEXT("Opacity"), 0.8);
	}
	else if (flow > startFlow * 0.7) {
		riverMaterialInstance->SetScalarParameterValue(TEXT("Opacity"), 0.7);
	}
	else if (flow > startFlow * 0.6) {
		riverMaterialInstance->SetScalarParameterValue(TEXT("Opacity"), 0.6);
	}
	else if (flow > startFlow * 0.5) {
		riverMaterialInstance->SetScalarParameterValue(TEXT("Opacity"), 0.55);
	}
	else if (flow > startFlow * 0.4) {
		riverMaterialInstance->SetScalarParameterValue(TEXT("Opacity"), 0.5);
	}
	else if (flow > startFlow * 0.3) {
		riverMaterialInstance->SetScalarParameterValue(TEXT("Opacity"), 0.45);
	}
	else if (flow > startFlow * 0.2) {
		riverMaterialInstance->SetScalarParameterValue(TEXT("Opacity"), 0.4);
	}
	else if (flow > 0.1f) {
		riverMaterialInstance->SetScalarParameterValue(TEXT("Opacity"), 0.35);
	}
	else if (flow > 0.0f) {
		riverMaterialInstance->SetScalarParameterValue(TEXT("Opacity"), 0.3);
	}
	else if (flow == 0.0f) {
		riverMaterialInstance->SetScalarParameterValue(TEXT("Opacity"), 0.0);
	}
	
	riverMesh->SetMaterial(0, riverMaterialInstance);	// update mesh material

}

// update flowers with new flow
void ARiverActor::waterFlowers() {
	float totalFlow = flow;

	for (int river = 0; river < overlappedRivers.Num(); river++) {
		totalFlow += overlappedRivers[river]->flow;	// accumulate all flow going to the node
	}

	bool flowerAlive = false;	// set initial flower goal status to incomplete

	for (int flowerIndex = 0; flowerIndex < flowerArray.Num(); flowerIndex++) {	// check if total flow meets flowers required flow
		if ( FMath::IsNearlyEqual(totalFlow, flowerArray[flowerIndex]->requiredFlow, 0.01f) ) {	// allow for precision error
			flowerArray[flowerIndex]->flowerMesh->SetMaterial(2, flowerArray[flowerIndex]->yellowRoseMat);	// change flower petal material to yellow
			flowerAlive = true;	// set flower goal to complete
		}
		else if ( totalFlow < flowerArray[flowerIndex]->requiredFlow){	// total flow is lower than required flow
			flowerArray[flowerIndex]->flowerMesh->SetMaterial(2, flowerArray[flowerIndex]->deadRoseMat);	// change flower petal material to white
		}
		else if (totalFlow > flowerArray[flowerIndex]->requiredFlow) {	// total flow is higher than required flow
			flowerArray[flowerIndex]->flowerMesh->SetMaterial(2, flowerArray[flowerIndex]->blueRoseMat);	// change flower petal material to blue
		}
	}

	// update the node goals completion status for this node at the last river actor
	for (int flowerNodeIndex = 0; flowerNodeIndex < finalNodeRiver->flowerNodeIDs.Num(); flowerNodeIndex++) {
		if (outputNode.Contains(finalNodeRiver->flowerNodeIDs[flowerNodeIndex]) ) {
			finalNodeRiver->nodeGoals[flowerNodeIndex] = flowerAlive;
		}
	}
	
}

// Method to check if level goal is completed 
void ARiverActor::checkLevelCompletion() {
	// check node goal completion status at the last river actor if all is set true
	if (!finalNodeRiver->nodeGoals.Contains(false)) {
		// delay level change for smoother transition
		FTimerHandle UnusedHandle;
		GetWorldTimerManager().SetTimer(
			UnusedHandle, this, &ARiverActor::changeLevel, 4.0f, false);
	}
}

// Method to change level
void ARiverActor::changeLevel() {
	UWorld* TheWorld = GetWorld();

	FString currentLevel = TheWorld->GetMapName();	// retrieve current level name

	// change to next level in numerical order or loop back to first level if on last level
	if (currentLevel.Contains("Level1")) {
		UGameplayStatics::OpenLevel(GetWorld(), "Level2");
	}
	else if (currentLevel.Contains("Level2")) {
		UGameplayStatics::OpenLevel(GetWorld(), "Level3");
	}
	else if (currentLevel.Contains("Level3")) {
		UGameplayStatics::OpenLevel(GetWorld(), "Level1");
	}
}