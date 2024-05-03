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
	void DeleteSetOfWall();
	void UndoLastWall();

	UPROPERTY()
	int currWall;

	UPROPERTY()
	bool bWasLeftClickLatest;

	UPROPERTY()
	int32 meshNo;

	UPROPERTY()
	AWallSpline* WallSpline;

	UPROPERTY()
	TArray<AWallSpline*> WallSplineArray;

	UPROPERTY()
	TArray<FVector> PointsLocation;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
public:
	AWallBuilderController();
};
