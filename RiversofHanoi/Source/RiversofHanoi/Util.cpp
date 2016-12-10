// Fill out your copyright notice in the Description page of Project Settings.

#include "RiversofHanoi.h"
#include "Util.h"

Util::Util(){
}

TArray<FVector> Util::normalise(TArray<FVector> normals) {

	for (int i = 0; i < normals.Num(); ++i) {
		float length = normals[i].Size();
		normals[i] = normals[i] / length;
	}
	return normals;
}

TArray<FVector> Util::findNormals(TArray<FVector> vertices, TArray<int32> Triangles) {
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

	tempNorm;

	return normalise(tempNorm);
}

void Util::readData(FString fileName, TArray<FVector> vertices, TArray<int32> Triangles){
	FString projectDir = FPaths::GameDir();
	projectDir += "Content/Levels/data/" + fileName;

	FString data = TEXT("");

	FFileHelper::LoadFileToString(data, *projectDir);

	TArray<FString> lines;
	TArray<FString> splitLines;

	int32 lineCount = data.ParseIntoArray(lines, _T("\n"), true);

	for (int32 Index = 3; Index < lines.Num(); ++Index) {
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

	verts = vertices;
	triangs = Triangles;

	norms = findNormals(vertices, Triangles);
}

void Util::readNodeData(FString fileName, TArray<FVector> nodeData)
{
    FString projectDir = FPaths::GameDir();
    projectDir += "Content/Levels/data/" + fileName;

    FString data = TEXT("");

    FFileHelper::LoadFileToString(data, *projectDir);

    TArray<FString> lines;
    TArray<FString> splitLines;

    int32 lineCount = data.ParseIntoArray(lines, _T("\n"), true);

    for (int32 Index = 0; Index < lines.Num(); ++Index) {
        lines[Index].ParseIntoArray(splitLines, _T(","), true);
        nodeData.Add(FVector(FCString::Atof(*splitLines[1]), FCString::Atof(*splitLines[0]), FCString::Atof(*splitLines[2])));
    }

    nodes = nodeData;
}

void Util::readRodData(FString fileName, TArray<FVector> rodData)
{
	FString projectDir = FPaths::GameDir();
	projectDir += "Content/Levels/data/" + fileName;

	FString data = TEXT("");

	FFileHelper::LoadFileToString(data, *projectDir);

	TArray<FString> lines;
	TArray<FString> rodsInNode;
	TArray<FString> rodCoord;

	int32 lineCount = data.ParseIntoArray(lines, _T("\n"), true);

	for (int32 Index = 0; Index < lines.Num(); ++Index) {
		if (lines[Index].TrimTrailing().IsEmpty()){ 
			rodData.Add(FVector(-1, -1, -1)); 
			if (Index > 1) {
				rodData.Add(FVector(-1, -1, -1));
			}
		}
		else {
			lines[Index].ParseIntoArray(rodsInNode, _T(" "), true);
			for (int32 rodIndex = 0; rodIndex < rodsInNode.Num(); ++rodIndex) {
					rodsInNode[rodIndex].ParseIntoArray(rodCoord, _T(","), true);
					rodData.Add(FVector(FCString::Atof(*rodCoord[1]), FCString::Atof(*rodCoord[0]), FCString::Atof(*rodCoord[2])));				
			}

		}
		
	}
	rods = rodData;
}

void Util::readRiverData(FString fileName, TArray<FVector> riverData)
{
	FString projectDir = FPaths::GameDir();
	projectDir += "Content/Levels/data/" + fileName;;

	FString data = TEXT("");

	FFileHelper::LoadFileToString(data, *projectDir);

	TArray<FString> lines;
	TArray<FString> riverVertices;
	TArray<FString> riverCoords;

	int32 lineCount = data.ParseIntoArray(lines, _T("\n"), true);

	for (int32 Index = 0; Index < lines.Num(); ++Index) {
		lines[Index] = lines[Index].TrimTrailing();
		lines[Index].ParseIntoArray(riverVertices, _T(" "), true);
		riverIDs.Add(riverVertices[4]);
		riverVertices.Pop();
		for (int32 vertex = 0; vertex < riverVertices.Num(); ++vertex) {
			riverVertices[vertex].ParseIntoArray(riverCoords, _T(","), true);
			riverData.Add(FVector(FCString::Atof(*riverCoords[1]), FCString::Atof(*riverCoords[0]), FCString::Atof(*riverCoords[2])));
		}
		TArray<FVector> vertices;
		vertices.Add(riverData[(Index * 4)]);
		vertices.Add(riverData[(Index * 4) + 1]);
		vertices.Add(riverData[(Index * 4) + 2]);
		vertices.Add(riverData[(Index * 4) + 3]);

	}

	rivers = riverData;
}

void Util::readRiverConnectionsData(FString fileName) {
	FString projectDir = FPaths::GameDir();
	projectDir += "Content/Levels/data/" + fileName;

	FString data = TEXT("");

	FFileHelper::LoadFileToString(data, *projectDir);

	TArray<FString> lines;
	TArray<FString> splitLines;

	data.ParseIntoArray(lines, _T("\n"), true);

	for (int32 Index = 0; Index < lines.Num(); ++Index) {
		lines[Index] = lines[Index].TrimTrailing();
		lines[Index].ParseIntoArray(splitLines, _T(" "), true);
		for (int32 river = 0; river < splitLines.Num(); ++river) {
			riverConnections.Add(splitLines[river]);
		}
		riverConnections.Add(TEXT("-1"));

	}
}

void Util::readRodRiverData(FString fileName) {

	FString projectDir = FPaths::GameDir();
	projectDir += "Content/Levels/data/" + fileName;

	FString data = TEXT("");

	FFileHelper::LoadFileToString(data, *projectDir);

	TArray<FString> lines;
	TArray<FString> riverInRod;

	int32 lineCount = data.ParseIntoArray(lines, _T("\n"), true);

	for (int32 Index = 0; Index < lines.Num(); ++Index) {
		lines[Index] = lines[Index].TrimTrailing();
		if (lines[Index].IsEmpty()) {
			rodRiverConnection.Add(TEXT("-1"));
			if (Index > 1) {
				rodRiverConnection.Add(TEXT("-1"));
			}
		}
		else {
			lines[Index].ParseIntoArray(riverInRod, _T(" "), true);
			for (int32 rodIndex = 0; rodIndex < riverInRod.Num(); ++rodIndex) {
				rodRiverConnection.Add(riverInRod[rodIndex]);
			}

		}

	}
}

void Util::readNodeConnectionsData(FString fileName) {
	FString projectDir = FPaths::GameDir();
	projectDir += "Content/Levels/data/" + fileName;

	FString data = TEXT("");

	FFileHelper::LoadFileToString(data, *projectDir);

	TArray<FString> lines;

	data.ParseIntoArray(lines, _T("\n"), true);

	for (int32 Index = 2; Index < lines.Num(); ++Index) {
		lines[Index] = lines[Index].TrimTrailing();
		if (lines[Index].IsEmpty()) {
			nodeRiverConnection.Add(TEXT("-1"));
		}
		else {
			nodeRiverConnection.Add(lines[Index]);
		}
	}
}