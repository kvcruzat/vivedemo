// Fill out your copyright notice in the Description page of Project Settings.

#include "RiversofHanoi.h"
#include "TerrainActor.h"

#include "ProceduralMeshComponent.h"

// Sets default values
ATerrainActor::ATerrainActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	/*PrimaryActorTick.bCanEverTick = true;*/

	readData("test.m");

	USphereComponent* SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	RootComponent = SphereComponent;

	UProceduralMeshComponent* terrainMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("TerrainMesh"));

	/*vertices.Add(FVector(-100.0, -100.0, -100.0));
	vertices.Add(FVector(-100.0, 100.0, 100.0));
	vertices.Add(FVector(100.0, -100.0, 100.0));
	vertices.Add(FVector(100.0, 100.0, -100.0));
	Triangles.Add(0);
	Triangles.Add(1);
	Triangles.Add(2);
	Triangles.Add(1);
	Triangles.Add(0);
	Triangles.Add(3);
	Triangles.Add(1);
	Triangles.Add(3);
	Triangles.Add(2);
	Triangles.Add(0);
	Triangles.Add(2);
	Triangles.Add(3);*/

	TArray<FVector> normals;
	normals.Add(FVector(1, 0, 0));
	normals.Add(FVector(1, 0, 0));
	normals.Add(FVector(1, 0, 0));

	TArray<FVector2D> UV0;
	UV0.Add(FVector2D(0, 0));
	UV0.Add(FVector2D(0, 10));
	UV0.Add(FVector2D(10, 10));

	TArray<FColor> vertexColors;
	vertexColors.Add(FColor(10, 100, 100, 100));
	vertexColors.Add(FColor(100, 50, 100, 100));
	vertexColors.Add(FColor(0, 100, 20, 100));

	TArray<FProcMeshTangent> tangents;
	tangents.Add(FProcMeshTangent(1, 1, 1));
	tangents.Add(FProcMeshTangent(1, 1, 1));
	tangents.Add(FProcMeshTangent(1, 1, 1));

	UE_LOG(LogTemp, Warning, TEXT("# Vertices: %s"), *FString::FromInt(vertices.Num()));
	UE_LOG(LogTemp, Warning, TEXT("# Faces: %s"), *FString::FromInt(Triangles.Num() / 3));

	//terrainMesh->CreateMeshSection(1, vertices, Triangles, normals, UV0, vertexColors, tangents, true);
	terrainMesh->CreateMeshSection(1, vertices, Triangles, TArray<FVector>(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), true);

	terrainMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ATerrainActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATerrainActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ATerrainActor::readData(FString fileName) {

	FString projectDir = FPaths::GameDir();
	projectDir += "Content/test.m";

	FString data = TEXT("");

	FFileHelper::LoadFileToString(data, *projectDir);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *data);

	TArray<FString> lines;
	TArray<FString> splitLines;

	int32 lineCount = data.ParseIntoArray(lines, _T("\n"), true);

	for (int32 Index = 3; Index < lines.Num(); ++Index) {
		UE_LOG(LogTemp, Warning, TEXT("%s"), *lines[Index]);
		if (lines[Index].StartsWith("Vertex")) {
			lines[Index].ParseIntoArray(splitLines, _T(" "), true);
			vertices.Add(FVector(FCString::Atof(*splitLines[2])*1000, FCString::Atof(*splitLines[3])*1000, FCString::Atof(*splitLines[4])*1000));
		}
		else if (lines[Index].StartsWith("Face")) {
			lines[Index].ParseIntoArray(splitLines, _T(" "), true);
			Triangles.Add(FCString::Atoi(*splitLines[2]));
			Triangles.Add(FCString::Atoi(*splitLines[3]));
			Triangles.Add(FCString::Atoi(*splitLines[4]));
		}
	}

	//ifstream datafile(filename.c_str());
	//QString qline;
	//QStringList splitLines;
	//size_t numV, numF;

	//if (not datafile.is_open()) {		//check for file
	//	cerr << "Error: cannot open file!" << endl;
	//	exit(EXIT_FAILURE);
	//}

	//string line;
	//if (datafile.is_open()) {
	//	for (int i = 0; i < 4; ++i) {
	//		getline(datafile, line);	// skips first 3 lines of header
	//	}
	//	getline(datafile, line);
	//	qline = QString::fromStdString(line);
	//	splitLines = qline.split(QRegExp("\\s+"));	// finds number of vertices and faces
	//	numV = splitLines[splitLines.indexOf(QRegExp("vertices.+"))].split("=")[1].toInt();
	//	numF = splitLines[splitLines.indexOf(QRegExp("faces.+"))].split("=")[1].toInt();
	//	getline(datafile, line);
	//	while (getline(datafile, line)) {
	//		qline = QString::fromStdString(line);
	//		splitLines = qline.split(QRegExp("\\s+"));
	//		if (splitLines[0] == "Vertex") {	// stores vertices
	//			Point p(splitLines[2].toFloat(),
	//				splitLines[3].toFloat(),
	//				splitLines[4].toFloat());
	//			vertices.push_back(p);
	//		}
	//		else if (splitLines[0] == "Face") {		// stores faces
	//			Vector v(splitLines[2].toFloat(),
	//				splitLines[3].toFloat(),
	//				splitLines[4].toFloat());
	//			faces.push_back(v);
	//		}
	//	}
	//	if (vertices.size() == numV && faces.size() == numF)	// checks size of stored data
	//		std::cout << "File input successful" << std::endl;
	//	else std::cout << "Error in reading data" << std::endl;

	//}

	//datafile.close();	// closes file
}

