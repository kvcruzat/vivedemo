// Fill out your copyright notice in the Description page of Project Settings.

#include "RiversofHanoi.h"
#include "TerrainActor.h"

#include "ProceduralMeshComponent.h"

// Sets default values
ATerrainActor::ATerrainActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	readData("terrain.m");

	USphereComponent* SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	RootComponent = SphereComponent;

	UProceduralMeshComponent* terrainMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("TerrainMesh"));

	UE_LOG(LogTemp, Warning, TEXT("# Vertices: %s"), *FString::FromInt(vertices.Num()));
	UE_LOG(LogTemp, Warning, TEXT("# Faces: %s"), *FString::FromInt(Triangles.Num() / 3));
	UE_LOG(LogTemp, Warning, TEXT("# Normals: %s"), *FString::FromInt(normals.Num()));

	//terrainMesh->CreateMeshSection(1, vertices, Triangles, normals, UV0, vertexColors, tangents, true);
	terrainMesh->CreateMeshSection(0, vertices, Triangles, normals, TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), true);

	terrainMesh->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void ATerrainActor::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ATerrainActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATerrainActor::readData(FString fileName) {

	FString projectDir = FPaths::GameDir();
	projectDir += "Content/models/" + fileName;

	FString data = TEXT("");

	FFileHelper::LoadFileToString(data, *projectDir);
	/*UE_LOG(LogTemp, Warning, TEXT("%s"), *data);*/

	TArray<FString> lines;
	TArray<FString> splitLines;

	int32 lineCount = data.ParseIntoArray(lines, _T("\n"), true);

	for (int32 Index = 3; Index < lines.Num(); ++Index) {
		/*UE_LOG(LogTemp, Warning, TEXT("%s"), *lines[Index]);*/
		if (lines[Index].StartsWith("Vertex")) {
			lines[Index].ParseIntoArray(splitLines, _T(" "), true);
			vertices.Add(FVector(FCString::Atof(*splitLines[2]), FCString::Atof(*splitLines[4]), FCString::Atof(*splitLines[3])));
		}
		else if (lines[Index].StartsWith("Face")) {
			lines[Index].ParseIntoArray(splitLines, _T(" "), true);
			Triangles.Add(FCString::Atoi(*splitLines[2]));
			Triangles.Add(FCString::Atoi(*splitLines[3]));
			Triangles.Add(FCString::Atoi(*splitLines[4]));
		}
	}

	findNormals();
}

void ATerrainActor::findNormals() {
	TArray<FVector> tempNorm;
	tempNorm.Init(FVector(0, 0, 0), vertices.Num());

	for (int i = 0; i < Triangles.Num(); i += 3) {

		FVector u = vertices[Triangles[i + 1]] - vertices[Triangles[i]];
		FVector v = vertices[Triangles[i + 2]] - vertices[Triangles[i]];
		tempNorm[Triangles[i]] = tempNorm[Triangles[i]] + FVector::CrossProduct(v, u);

		FVector w = vertices[Triangles[i]] - vertices[Triangles[i + 1]];
		FVector x = vertices[Triangles[i + 2]] - vertices[Triangles[i + 1]];
		tempNorm[Triangles[i + 1]] = tempNorm[Triangles[i + 1]] + FVector::CrossProduct(w, x);

		FVector y = vertices[Triangles[i]] - vertices[Triangles[i + 2]];
		FVector z = vertices[Triangles[i + 1]] - vertices[Triangles[i + 2]];
		tempNorm[Triangles[i + 2]] = tempNorm[Triangles[i + 2]] + FVector::CrossProduct(z, y);
	}

	normals = tempNorm;

	//UE_LOG(LogTemp, Warning, TEXT("%s"), *normals[0].ToString());
	//UE_LOG(LogTemp, Warning, TEXT("%s"), *normals[1].ToString());
	//UE_LOG(LogTemp, Warning, TEXT("%s"), *normals[2].ToString());

	normalise();
}

void ATerrainActor::normalise() {

	for (int i = 0; i < normals.Num(); ++i) {
		float length = normals[i].Size();
		normals[i] = normals[i] / length;
	}

	/*UE_LOG(LogTemp, Warning, TEXT("%s"), *normals[0].ToString());
	UE_LOG(LogTemp, Warning, TEXT("%s"), *normals[1].ToString());
	UE_LOG(LogTemp, Warning, TEXT("%s"), *normals[2].ToString());*/
}