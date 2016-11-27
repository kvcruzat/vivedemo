// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/**
 * 
 */
class RIVERSOFHANOI_API Util
{
public:
	Util();

	TArray<FVector> verts;
	TArray<FVector> norms;
	TArray<int32> triangs;

    TArray<FVector> nodes;
	TArray<FVector> rods;

	TArray<FVector> rivers;
	TArray<FVector> riverNorms;
	
	void readData(FString fileName, TArray<FVector> vertices, TArray<int32> Triangles);
	TArray<FVector> findNormals(TArray<FVector> vertices, TArray<int32> Triangles);
	TArray<FVector> normalise(TArray<FVector> normals);
    void readNodeData(FString fileName, TArray<FVector> nodeData);
	void readRodData(FString fileName, TArray<FVector> rodData);
	void readRiverData(FString fileName, TArray<FVector> riverData);
};
