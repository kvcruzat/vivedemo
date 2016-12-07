// Fill out your copyright notice in the Description page of Project Settings.

#include "RiversofHanoi.h"
#include "RiverActor.h"
#include "Util.h"


// Sets default values
ARiverActor::ARiverActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	flow = 0;

	discStatus = 0;

	initialSet = true;

	outputNode = TEXT("");

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

void ARiverActor::createMesh(TArray<FVector> vertexData) {
	TArray<int32> triangles;

	triangles.Add(2); //0
	triangles.Add(1); //1
	triangles.Add(0); //2
	triangles.Add(0); //2
	triangles.Add(3); //3
	triangles.Add(2); //0

	Util *util = new Util();
	normals = util->findNormals(vertexData, triangles);

	riverMesh->CreateMeshSection(0, vertexData, triangles, normals, TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), false);
	
	riverMesh->SetMaterial(0, riverMaterial);
	riverMesh->SetCollisionProfileName(FName(TEXT("OverlapAll")));
	riverMaterialInstance = riverMesh->CreateDynamicMaterialInstance(0);
	
}

void ARiverActor::changeFlow(float value) {

	flow += value;

	if (connectedRivers.Num() == 0) {
		float totalFlow = flow;

		for (int river = 0; river < overlappedRivers.Num(); river++) {
			totalFlow += overlappedRivers[river]->flow;
		}

		float incomingFlow = totalFlow / 2;

		float flowDiff = 0;

		for (int river = 0; river < outputRivers.Num(); river++) {
			flowDiff = (incomingFlow + (incomingFlow * outputRivers[river]->discStatus)) - outputRivers[river]->flow;
			outputRivers[river]->changeFlow(flowDiff);
		}

	}
	else {
		for (int Index = 0; Index < connectedRivers.Num(); Index++) {

			connectedRivers[Index]->changeFlow(value);

			if (Index == connectedRivers.Num() - 1 && !connectedRivers[Index]->outputNode.IsEmpty()) {
				float totalFlow = connectedRivers[Index]->flow;

				for (int river = 0; river < connectedRivers[Index]->overlappedRivers.Num(); river++) {
					totalFlow += connectedRivers[Index]->overlappedRivers[river]->flow;
				}

				float incomingFlow = totalFlow / 2;

				float flowDiff = 0;

				for (int river = 0; river < connectedRivers[Index]->outputRivers.Num(); river++) {
					flowDiff = (incomingFlow + (incomingFlow * connectedRivers[Index]->outputRivers[river]->discStatus)) - connectedRivers[Index]->outputRivers[river]->flow;
					connectedRivers[Index]->outputRivers[river]->changeFlow(flowDiff);
				}

			}

		}
	}

	float startFlow = 24;

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
		riverMaterialInstance->SetScalarParameterValue(TEXT("Opacity"), 0.65);
	}
	else if (flow > startFlow * 0.5) {
		riverMaterialInstance->SetScalarParameterValue(TEXT("Opacity"), 0.6);
	}
	else if (flow > startFlow * 0.4) {
		riverMaterialInstance->SetScalarParameterValue(TEXT("Opacity"), 0.55);
	}
	else if (flow > startFlow * 0.3) {
		riverMaterialInstance->SetScalarParameterValue(TEXT("Opacity"), 0.5);
	}
	else if (flow > startFlow * 0.2) {
		riverMaterialInstance->SetScalarParameterValue(TEXT("Opacity"), 0.45);
	}
	else if (flow > 0.0f) {
		riverMaterialInstance->SetScalarParameterValue(TEXT("Opacity"), 0.4);
	}
	else if (flow == 0.0f) {
		riverMaterialInstance->SetScalarParameterValue(TEXT("Opacity"), 0.0);
	}
	
	riverMesh->SetMaterial(0, riverMaterialInstance);

	float opacity;
	riverMaterialInstance->GetScalarParameterValue(TEXT("Opacity"), opacity);
	
	UE_LOG(LogTemp, Warning, TEXT("# river%s: %s, %s"), *riverID, *FString::SanitizeFloat(flow), *FString::SanitizeFloat(opacity));

}

void ARiverActor::waterFlowers(float value) {

	for (int flowerIndex = 0; flowerIndex < flowerArray.Num(); flowerIndex++) {
		if (flowerArray[flowerIndex]->requiredFlow == value) {
		}
	}
}