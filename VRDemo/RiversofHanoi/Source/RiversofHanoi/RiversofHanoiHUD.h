// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once 
#include "GameFramework/HUD.h"
#include "RiversofHanoiHUD.generated.h"

UCLASS()
class ARiversofHanoiHUD : public AHUD
{
	GENERATED_BODY()

public:
	ARiversofHanoiHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

