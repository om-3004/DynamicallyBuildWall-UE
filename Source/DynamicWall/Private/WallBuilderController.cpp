// Fill out your copyright notice in the Description page of Project Settings.


#include "WallBuilderController.h"

#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

AWallBuilderController::AWallBuilderController()
{
	WallSpline = CreateDefaultSubobject<AWallSpline>(TEXT("Wall Spline 0"));
	WallSpline->SplineStaticMesh = LoadObject<UStaticMesh>(this, TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/Architecture/Wall_400x200.Wall_400x200'"));
}

void AWallBuilderController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = true;
}

void AWallBuilderController::BuildWall()
{
	FHitResult HitResult;
	GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
	if (HitResult.bBlockingHit)
	{
		FVector ClickedLocation = HitResult.Location;
		WallSpline->GenerateWall(ClickedLocation);
	}
}

void AWallBuilderController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UInputMappingContext* InputMappingContext = NewObject<UInputMappingContext>();

	UInputAction* CreateWall = NewObject<UInputAction>();
	CreateWall->ValueType = EInputActionValueType::Boolean;
	InputMappingContext->MapKey(CreateWall, EKeys::LeftMouseButton);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	EnhancedInputComponent->BindAction(CreateWall, ETriggerEvent::Completed, this, &AWallBuilderController::BuildWall);

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer())) {
		Subsystem->AddMappingContext(InputMappingContext, 0);
	}
}

