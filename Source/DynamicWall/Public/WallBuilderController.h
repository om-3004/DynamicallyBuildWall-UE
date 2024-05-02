// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "WallSpline.h"

#include "WallBuilderController.generated.h"

/**
 * 
 */
UCLASS()
class DYNAMICWALL_API AWallBuilderController : public APlayerController
{
	GENERATED_BODY()
	
	void BuildWall();
	void BuildNewWall();
	void GoToPreviousWall();
	void GoToNextWall();

	UPROPERTY()
	int currWall;

	UPROPERTY()
	bool bWasLeftClickLatest;

	UPROPERTY()
	AWallSpline* WallSpline;

	UPROPERTY()
	TArray<AWallSpline*> WallSplineArray;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
public:
	AWallBuilderController();
};
