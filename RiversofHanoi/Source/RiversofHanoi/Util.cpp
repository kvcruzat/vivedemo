// Fill out your copyright notice in the Description page of Project Settings.

#include "RiversofHanoi.h"
#include "Util.h"

Util::Util(){
}

void Util::normalise(TArray<FVector> normals) {

	for (int i = 0; i < normals.Num(); ++i) {
		float length = normals[i].Size();
		normals[i] = normals[i] / length;
	}
	norms = normals;
}

void Util::findNormals(TArray<FVector> vertices, TArray<FVector> normals, TArray<int32> Triangles) {
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

	normalise(normals);
}

void Util::readData(FString fileName, TArray<FVector> vertices, TArray<FVector> normals, TArray<int32> Triangles){
	FString projectDir = FPaths::GameDir();
	projectDir += "Content/models/" + fileName;

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

	findNormals(vertices, normals, Triangles);
}

