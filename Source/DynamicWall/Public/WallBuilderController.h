// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "WallSpline.h"

#include "WallBuilderController.generated.h"

DECLARE_DELEGATE_TwoParams(PrintMessage, const FString&, const FString&)

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
	void DeleteEveryWall();

	UPROPERTY()
	int currWall;

	UPROPERTY()
	int32 meshNo;

	UPROPERTY()
	AWallSpline* WallSpline;

	UPROPERTY()
	TArray<AWallSpline*> WallSplineArray;

	PrintMessage delegateMsg;

protected:

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
public:
	UFUNCTION(BlueprintImplementableEvent)
	void ShowInViewPort(const FString& InfoMsg, const FString& ErrorMsg);

	AWallBuilderController();
};
