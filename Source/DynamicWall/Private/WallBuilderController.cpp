// Fill out your copyright notice in the Description page of Project Settings.


#include "WallBuilderController.h"

#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

AWallBuilderController::AWallBuilderController() : currWall{ 0 }
{
	meshNo = 0;
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
	if (WallSplineArray[currWall]->SplineComponent->GetNumberOfSplinePoints() == 0) {
		delegateMsg.Execute(FString{ "" }, FString{ "Create a wall in the existing segment before creating a new segment of walls." });
	}
	else if (currWall < WallSplineArray.Num() - 1) {
		delegateMsg.Execute(FString{ "" }, FString{ "Go to the latest wall before creating a new segment of walls." });
	}
	else {
		FHitResult HitResult;
		GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
		if (HitResult.bBlockingHit) {
			currWall++;
			meshNo++;
			FString splineName = "Wall Spline " + FString::FromInt(meshNo);
			WallSpline = NewObject<AWallSpline>(this, AWallSpline::StaticClass(), *splineName);
			WallSpline->SplineStaticMesh = LoadObject<UStaticMesh>(this, TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/Architecture/Wall_400x200.Wall_400x200'"));
			WallSplineArray.Add(WallSpline);
			delegateMsg.Execute(FString{ "Created a new wall segment. Click and select 2 points to build first wall." }, FString{ "" });
		}
	}
}

void AWallBuilderController::GoToPreviousWall()
{
	if (currWall > 0) {
		currWall--;
		delegateMsg.Execute(FString{ "Switched to previous wall segment" }, FString{ "" });
	}
	else {
		delegateMsg.Execute(FString{ "" }, FString{ "Already on the starting wall. Can't go previous from this segment of walls." });
	}
}

void AWallBuilderController::GoToNextWall()
{
	if (currWall < WallSplineArray.Num() - 1) {
		currWall++;
		delegateMsg.Execute(FString{ "Switched to next wall segment" }, FString{ "" });
	}
	else {
		delegateMsg.Execute(FString{ "" }, FString{ "Already on the latest wall. Can't go next from this segment of walls." });
	}
}

void AWallBuilderController::DeleteSegmentOfWall()
{
	WallSplineArray[currWall]->SplineComponent->ClearSplinePoints();
	WallSplineArray[currWall]->deleteComponents();
	WallSplineArray[currWall]->Destroy();
	if (currWall != WallSplineArray.Num() - 1) {
		WallSplineArray.RemoveAt(currWall);
		delegateMsg.Execute(FString{ "Destroyed this segment of wall. The next segment of wall is automatically selected." }, FString{ "" });
	}
	else if (currWall == 0 && WallSplineArray.Num() == 1) {
		delegateMsg.Execute(FString{ "Destroyed this segment of wall. The next segment of wall is automatically selected." }, FString{ "" });
	}
	else {
		WallSplineArray.RemoveAt(currWall);
		currWall--;
		delegateMsg.Execute(FString{ "Destroyed this segment of wall. The previous segment of wall is automatically selected." }, FString{ "" });
	}
}

void AWallBuilderController::UndoLastWall()
{
	if(WallSplineArray.Num() > 0 && currWall >= 0) {
		int32 noOfPts = WallSplineArray[currWall]->SplineComponent->GetNumberOfSplinePoints();

		if (noOfPts > 2) {
			WallSplineArray[currWall]->SplineComponent->RemoveSplinePoint(noOfPts - 1);

			WallSplineArray[currWall]->deleteLastWall();
			WallSplineArray[currWall]->GenerateSplineMeshComponents();
			delegateMsg.Execute(FString{ "Undid creation of the last wall in the current segment of walls." }, FString{ "" });
		}
		else if (noOfPts == 2) {
			WallSplineArray[currWall]->SplineComponent->RemoveSplinePoint(noOfPts - 1);
			WallSplineArray[currWall]->SplineComponent->RemoveSplinePoint(noOfPts - 2);

			WallSplineArray[currWall]->deleteLastWall();
			WallSplineArray[currWall]->GenerateSplineMeshComponents();

			if (WallSplineArray.Num() == 1 && currWall == 0) { // Only 1 wall segment
				delegateMsg.Execute(FString{ "Undid creation of the last wall in the current segment of walls. Created a new wall segment." }, FString{ "" });
			}
			else if (currWall < WallSplineArray.Num() - 1) { // Not latest wall segment
				WallSplineArray.RemoveAt(currWall);

				delegateMsg.Execute(FString{ "Undid creation of the last wall in the current segment of walls. Switched to next wall segment." }, FString{ "" });
			}
			else if(currWall == WallSplineArray.Num() - 1) { // Latest wall segment
				WallSplineArray.RemoveAt(currWall);
				currWall--;

				delegateMsg.Execute(FString{ "Undid creation of the last wall in the current segment of walls. Switched to previous wall segment." }, FString{ "" });
			}
		}
		else {
			delegateMsg.Execute(FString{ "" }, FString{ "Not enough spline points to undo last wall in the current segment of walls." });
		}
	}
	else {
		delegateMsg.Execute(FString{ "" }, FString{ "No walls to undo" });
	}
}

void AWallBuilderController::DeleteEveryWall()
{

	int32 n = WallSplineArray.Num();
	for (int i = 0; i < n; i++) {
		WallSplineArray[i]->SplineComponent->ClearSplinePoints();
		WallSplineArray[i]->deleteComponents();
		WallSplineArray[i]->Destroy();
		if (i != n - 1) {
			delegateMsg.Execute(FString{ "Destroyed all segments of wall. A new segment of wall is automatically selected." }, FString{ "" });
		}
		else if (i == 0 && n == 1) {
			delegateMsg.Execute(FString{ "Destroyed all segments of wall. A new segment of wall is automatically selected." }, FString{ "" });
		}
		else {
			delegateMsg.Execute(FString{ "Destroyed all segments of wall. A new segment of wall is automatically selected." }, FString{ "" });
		}
	}

	for (int i = 1; i < n; i++) {
		WallSplineArray.RemoveAt(1);
	}
	currWall = 0;

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

	UInputAction* DestroySegmentOfWall = NewObject<UInputAction>();
	DestroySegmentOfWall->ValueType = EInputActionValueType::Boolean;
	InputMappingContext->MapKey(DestroySegmentOfWall, EKeys::B);

	UInputAction* UndoWall = NewObject<UInputAction>();
	UndoWall->ValueType = EInputActionValueType::Boolean;
	InputMappingContext->MapKey(UndoWall, EKeys::Z);

	UInputAction* DeleteAllSegmentOfWalls = NewObject<UInputAction>();
	DeleteAllSegmentOfWalls->ValueType = EInputActionValueType::Boolean;
	InputMappingContext->MapKey(DeleteAllSegmentOfWalls, EKeys::C);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	EnhancedInputComponent->BindAction(CreateWall, ETriggerEvent::Completed, this, &AWallBuilderController::BuildWall);
	EnhancedInputComponent->BindAction(NewWall, ETriggerEvent::Completed, this, &AWallBuilderController::BuildNewWall);
	EnhancedInputComponent->BindAction(PreviousWall, ETriggerEvent::Completed, this, &AWallBuilderController::GoToPreviousWall);
	EnhancedInputComponent->BindAction(NextWall, ETriggerEvent::Completed, this, &AWallBuilderController::GoToNextWall);
	EnhancedInputComponent->BindAction(DestroySegmentOfWall, ETriggerEvent::Completed, this, &AWallBuilderController::DeleteSegmentOfWall);
	EnhancedInputComponent->BindAction(UndoWall, ETriggerEvent::Completed, this, &AWallBuilderController::UndoLastWall);
	EnhancedInputComponent->BindAction(DeleteAllSegmentOfWalls, ETriggerEvent::Completed, this, &AWallBuilderController::DeleteEveryWall);

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer())) {
		Subsystem->AddMappingContext(InputMappingContext, 0);
	}
}

