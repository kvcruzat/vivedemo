// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "RiversofHanoi.h"
#include "RiversofHanoiGameMode.h"
#include "RiversofHanoiHUD.h"
#include "RiversofHanoiCharacter.h"

ARiversofHanoiGameMode::ARiversofHanoiGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ARiversofHanoiHUD::StaticClass();
}
