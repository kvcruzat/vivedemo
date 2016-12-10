// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "TransitionWidget.generated.h"

/**
 * 
 */
UCLASS()
class RIVERSOFHANOI_API UTransitionWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UWidgetAnimation* levelTransAnim;
	
	
};
