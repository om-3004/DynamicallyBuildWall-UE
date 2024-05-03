// Fill out your copyright notice in the Description page of Project Settings.


#include "WallBuilderController.h"

#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

AWallBuilderController::AWallBuilderController() : currWall{ 0 }
{
	FString SplineName = FString::Printf(TEXT("Wall Spline %d"), meshNo);
	meshNo++;
	WallSpline = CreateDefaultSubobject<AWallSpline>(*SplineName);

	//WallSpline = CreateDefaultSubobject<AWallSpline>(TEXT("Wall Spline 0"));
	WallSplineArray.Add(WallSpline);
	WallSpline->SplineStaticMesh = LoadObject<UStaticMesh>(this, TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/Architecture/Wall_400x200.Wall_400x200'"));
}

void AWallBuilderController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = true;
}

void AWallBuilderController::BuildWall()
{
	bWasLeftClickLatest = false;
	FHitResult HitResult;
	GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
	if (HitResult.bBlockingHit)
	{
		FVector ClickedLocation = HitResult.Location;
		//PointsLocation[currWall] = HitResult.Location;
		if (WallSplineArray[currWall]) {
			WallSplineArray[currWall]->GenerateWall(ClickedLocation);
		}
	}
}

void AWallBuilderController::BuildNewWall()
{
	if (bWasLeftClickLatest) {
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, "Create a wall before going on to the next wall.");
	}
	else {
		bWasLeftClickLatest = true;
		FHitResult HitResult;
		GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
		if (HitResult.bBlockingHit) {
			currWall++;
			FString splineName = "Wall Spline " + FString::FromInt(meshNo);
			meshNo++;
			//FString splineName = "Wall Spline " + FString::FromInt(currWall);
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
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Magenta, "Already on the starting wall. Can't go further previous");
	}
}

void AWallBuilderController::GoToNextWall()
{
	if (currWall < WallSplineArray.Num() - 1) {
		currWall++;
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Magenta, "Already on the latest wall. Can't go further next");
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
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Magenta, "Destroyed this set of wall. The next set of wall is automatically selected.");
}

void AWallBuilderController::UndoLastWall()
{
	if(WallSplineArray.Num() > 0 && currWall >= 0) {
		int32 noOfPts = WallSplineArray[currWall]->SplineComponent->GetNumberOfSplinePoints();

		if (noOfPts > 2) {
			WallSplineArray[currWall]->SplineComponent->RemoveSplinePoint(noOfPts - 1);
			//WallSplineArray[currWall]->SplineComponent->RemoveSplinePoint(noOfPts - 2);

			WallSplineArray[currWall]->deleteLastWall();
			WallSplineArray[currWall]->GenerateSplineMeshComponents();
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Magenta, "Undid creation of the last wall");
		}
		else if (noOfPts == 2) {
			WallSplineArray[currWall]->SplineComponent->RemoveSplinePoint(noOfPts - 1);
			WallSplineArray[currWall]->SplineComponent->RemoveSplinePoint(noOfPts - 2);
			WallSplineArray[currWall]->deleteLastWall();
			WallSplineArray[currWall]->GenerateSplineMeshComponents();

			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Magenta, "Undid creation of the last wall");
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Magenta, "Not enough spline points to undo last wall");
		}
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Magenta, "No walls to undo");
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

