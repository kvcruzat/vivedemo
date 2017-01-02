// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/**
 *	Utility class header file
 */
class RIVERSOFHANOI_API Util
{
public:
	Util();

	// Arrays that can be accessed to retrieve data read
	TArray<FVector> verts;
	TArray<FVector> norms;
	TArray<int32> triangs;

    TArray<FVector> nodes;
	TArray<FVector> rods;

	TArray<FVector> rivers;
	TArray<FVector> riverNorms;
	TArray<FString> riverIDs;
	TArray<FString> riverConnections;
	TArray<FString> rodRiverConnection;
	TArray<FString> nodeRiverConnection;

	// Methods to read in text data files for levels
	void readData(FString fileName, TArray<FVector> vertices, TArray<int32> Triangles);
    void readNodeData(FString fileName, TArray<FVector> nodeData);
	void readRodData(FString fileName, TArray<FVector> rodData);
	void readRiverData(FString fileName, TArray<FVector> riverData);
	void readRiverConnectionsData(FString fileName);
	void readRodRiverData(FString fileName);
	void readNodeConnectionsData(FString fileName);

	// Methods to compute normals and to normalise vectors
	TArray<FVector> findNormals(TArray<FVector> vertices, TArray<int32> Triangles);
	TArray<FVector> normalise(TArray<FVector> normals);
};
