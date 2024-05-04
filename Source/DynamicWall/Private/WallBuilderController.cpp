// Fill out your copyright notice in the Description page of Project Settings.


#include "WallBuilderController.h"

#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

AWallBuilderController::AWallBuilderController() : currWall{ 0 }
{
	meshNo = 0;
	//FString SplineName = FString::Printf(TEXT("Wall Spline %d"), meshNo);
	//meshNo++;
	//WallSpline = CreateDefaultSubobject<AWallSpline>(*SplineName);
	WallSpline = CreateDefaultSubobject<AWallSpline>(TEXT("Wall Spline 0"));

	WallSpline->SplineStaticMesh = LoadObject<UStaticMesh>(this, TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/Architecture/Wall_400x200.Wall_400x200'"));
	WallSplineArray.Empty();
	WallSplineArray.Add(WallSpline);
}

void AWallBuilderController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = true;
	delegateMsg.BindUFunction(this, FName{ "ShowInViewPort" });
}

void AWallBuilderController::BuildWall()
{
	bWasLeftClickLatest = false;
	FHitResult HitResult;
	GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
	if (HitResult.bBlockingHit)
	{
		FVector ClickedLocation = HitResult.Location;
		if (WallSplineArray[currWall]) {
			WallSplineArray[currWall]->GenerateWall(ClickedLocation);
		}
	}
}

void AWallBuilderController::BuildNewWall()
{
	if (bWasLeftClickLatest) {
		delegateMsg.Execute(FString{ "" }, FString{ "Create a wall before in the existing set before creating a new set of walls." });
	}
	else if (currWall < WallSplineArray.Num() - 1) {
		delegateMsg.Execute(FString{ "" }, FString{ "Go to the latest wall before creating a new set of walls." });
	}
	else {
		bWasLeftClickLatest = true;
		FHitResult HitResult;
		GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
		if (HitResult.bBlockingHit) {
			currWall++;
			meshNo++;
			FString splineName = "Wall Spline " + FString::FromInt(meshNo);
			WallSpline = NewObject<AWallSpline>(this, AWallSpline::StaticClass(), *splineName);
			WallSpline->SplineStaticMesh = LoadObject<UStaticMesh>(this, TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/Architecture/Wall_400x200.Wall_400x200'"));
			WallSplineArray.Add(WallSpline);
		}
	}
}

void AWallBuilderController::GoToPreviousWall()
{
	if (currWall > 0) {
		currWall--;
		delegateMsg.Execute(FString{ "Switched to previous wall set" }, FString{ "" });
	}
	else {
		delegateMsg.Execute(FString{ "" }, FString{ "Already on the starting wall. Can't go previous from this set of walls." });
	}
}

void AWallBuilderController::GoToNextWall()
{
	if (currWall < WallSplineArray.Num() - 1) {
		currWall++;
		delegateMsg.Execute(FString{ "Switched to next wall set" }, FString{ "" });
	}
	else {
		delegateMsg.Execute(FString{ "" }, FString{ "Already on the latest wall. Can't go next from this set of walls." });
	}
}

void AWallBuilderController::DeleteSetOfWall()
{
	WallSplineArray[currWall]->SplineComponent->ClearSplinePoints();
	WallSplineArray[currWall]->deleteComponents();
	WallSplineArray[currWall]->Destroy();
	if (currWall != WallSplineArray.Num() - 1) {
		WallSplineArray.RemoveAt(currWall);
	}
	delegateMsg.Execute(FString{ "Destroyed this set of wall. The next set of wall is automatically selected." }, FString{ "" });
}

void AWallBuilderController::UndoLastWall()
{
	if(WallSplineArray.Num() > 0 && currWall >= 0) {
		int32 noOfPts = WallSplineArray[currWall]->SplineComponent->GetNumberOfSplinePoints();

		if (noOfPts > 2) {
			WallSplineArray[currWall]->SplineComponent->RemoveSplinePoint(noOfPts - 1);

			WallSplineArray[currWall]->deleteLastWall();
			WallSplineArray[currWall]->GenerateSplineMeshComponents();
			delegateMsg.Execute(FString{ "Undid creation of the last wall in the current set of walls." }, FString{ "" });
		}
		else if (noOfPts == 2) {
			WallSplineArray[currWall]->SplineComponent->RemoveSplinePoint(noOfPts - 1);
			WallSplineArray[currWall]->SplineComponent->RemoveSplinePoint(noOfPts - 2);

			WallSplineArray[currWall]->deleteLastWall();
			WallSplineArray[currWall]->GenerateSplineMeshComponents();

			delegateMsg.Execute(FString{ "Undid creation of the last wall in the current set of walls." }, FString{ "" });
		}
		else {
			delegateMsg.Execute(FString{ "" }, FString{ "Not enough spline points to undo last wall in the current set of walls." });
		}
	}
	else {
		delegateMsg.Execute(FString{ "" }, FString{ "No walls to undo" });
	}
}

void AWallBuilderController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UInputMappingContext* InputMappingContext = NewObject<UInputMappingContext>();

	UInputAction* CreateWall = NewObject<UInputAction>();
	CreateWall->ValueType = EInputActionValueType::Boolean;
	InputMappingContext->MapKey(CreateWall, EKeys::LeftMouseButton);

	UInputAction* NewWall = NewObject<UInputAction>();
	NewWall->ValueType = EInputActionValueType::Boolean;
	InputMappingContext->MapKey(NewWall, EKeys::RightMouseButton);

	UInputAction* PreviousWall = NewObject<UInputAction>();
	PreviousWall->ValueType = EInputActionValueType::Boolean;
	InputMappingContext->MapKey(PreviousWall, EKeys::P);

	UInputAction* NextWall = NewObject<UInputAction>();
	NextWall->ValueType = EInputActionValueType::Boolean;
	InputMappingContext->MapKey(NextWall, EKeys::N);

	UInputAction* DestroySetOfWall = NewObject<UInputAction>();
	DestroySetOfWall->ValueType = EInputActionValueType::Boolean;
	InputMappingContext->MapKey(DestroySetOfWall, EKeys::B);

	UInputAction* UndoWall = NewObject<UInputAction>();
	UndoWall->ValueType = EInputActionValueType::Boolean;
	InputMappingContext->MapKey(UndoWall, EKeys::Z);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	EnhancedInputComponent->BindAction(CreateWall, ETriggerEvent::Completed, this, &AWallBuilderController::BuildWall);
	EnhancedInputComponent->BindAction(NewWall, ETriggerEvent::Completed, this, &AWallBuilderController::BuildNewWall);
	EnhancedInputComponent->BindAction(PreviousWall, ETriggerEvent::Completed, this, &AWallBuilderController::GoToPreviousWall);
	EnhancedInputComponent->BindAction(NextWall, ETriggerEvent::Completed, this, &AWallBuilderController::GoToNextWall);
	EnhancedInputComponent->BindAction(DestroySetOfWall, ETriggerEvent::Completed, this, &AWallBuilderController::DeleteSetOfWall);
	EnhancedInputComponent->BindAction(UndoWall, ETriggerEvent::Completed, this, &AWallBuilderController::UndoLastWall);

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer())) {
		Subsystem->AddMappingContext(InputMappingContext, 0);
	}
}

