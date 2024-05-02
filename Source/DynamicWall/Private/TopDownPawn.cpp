// Fill out your copyright notice in the Description page of Project Settings.


#include "TopDownPawn.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATopDownPawn::ATopDownPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	CapsuleComponent->SetCapsuleHalfHeight(90);
	CapsuleComponent->SetCapsuleRadius(40);
	CapsuleComponent->SetEnableGravity(false);
	RootComponent = CapsuleComponent;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = -211.599960;
	SpringArmComponent->TargetOffset = FVector(-520, 0, 540);
	SpringArmComponent->bUsePawnControlRotation = true;

	float Angle = FMath::RadiansToDegrees(atan(SpringArmComponent->TargetOffset.Z / SpringArmComponent->TargetArmLength));

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetRelativeRotation(FRotator(-Angle, 0, 0));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);

	FloatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FloatingPawnMovement"));
}

// Called when the game starts or when spawned
void ATopDownPawn::BeginPlay()
{
	Super::BeginPlay();
}


void ATopDownPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (!CameraComponent) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Camera null"));
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		UInputMappingContext* InputMappingContext = NewObject<UInputMappingContext>();

		UInputAction* MoveAction = NewObject<UInputAction>();
		MoveAction->ValueType = EInputActionValueType::Axis2D;

		UInputModifierSwizzleAxis* YXZSwizzleAxisModifier = NewObject<UInputModifierSwizzleAxis>();
		YXZSwizzleAxisModifier->Order = EInputAxisSwizzle::YXZ;

		UInputModifierNegate* MoveActionNegateModifier = NewObject<UInputModifierNegate>();
		MoveActionNegateModifier->bX = true;
		MoveActionNegateModifier->bY = true;
		MoveActionNegateModifier->bZ = true;

		InputMappingContext->MapKey(MoveAction, EKeys::W);

		FEnhancedActionKeyMapping& MoveActionBackwardKeyMapping = InputMappingContext->MapKey(MoveAction, EKeys::S);
		MoveActionBackwardKeyMapping.Modifiers.Add(MoveActionNegateModifier);

		FEnhancedActionKeyMapping& MoveActionRightKeyMapping = InputMappingContext->MapKey(MoveAction, EKeys::D);
		MoveActionRightKeyMapping.Modifiers.Add(YXZSwizzleAxisModifier);

		FEnhancedActionKeyMapping& MoveActionLeftKeyMapping = InputMappingContext->MapKey(MoveAction, EKeys::A);
		MoveActionLeftKeyMapping.Modifiers.Add(YXZSwizzleAxisModifier);
		MoveActionLeftKeyMapping.Modifiers.Add(MoveActionNegateModifier);

		//--------------- Zoom Action ---------------------//
		UInputAction* ZoomAction = NewObject<UInputAction>();
		ZoomAction->ValueType = EInputActionValueType::Axis1D;

		FEnhancedActionKeyMapping& ZoomMapping = InputMappingContext->MapKey(ZoomAction, EKeys::MouseWheelAxis);
		//ZoomMapping.Scale = 1.0f;
		EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &ATopDownPawn::Zoom);
		
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATopDownPawn::Move);

		if (const APlayerController* PlayerController = Cast<APlayerController>(Controller)) {
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) {
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}

	}
}

void ATopDownPawn::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// get forward vector
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	// get right vector 
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	// add movement 
	AddMovementInput(ForwardDirection, MovementVector.X);
	AddMovementInput(RightDirection, MovementVector.Y);
}

void ATopDownPawn::Zoom(const FInputActionValue& Value) {

	if(CameraComponent) {
		float ZoomDelta = Value.Get<float>(); // 1 / -1
		
		float TargetArmLength = SpringArmComponent->TargetArmLength;
		float TargetOffsetZ = SpringArmComponent->TargetOffset.Z;

		if (TargetArmLength >= -460) {
			SpringArmComponent->TargetOffset.Z -= ZoomDelta * 20;
		}
		if (TargetOffsetZ <= 1000) {
			SpringArmComponent->TargetArmLength -= ZoomDelta * 9.2;
		}
		
		SpringArmComponent->TargetArmLength = FMath::Clamp(SpringArmComponent->TargetArmLength, -500, 0);
		SpringArmComponent->TargetOffset.Z = FMath::Clamp(SpringArmComponent->TargetOffset.Z, 0, 1500);

		FString FloatAsString = FString::SanitizeFloat(SpringArmComponent->TargetArmLength);
		FString DebugMessage = FString::Printf(TEXT("MyFloatValue: %s"), *FloatAsString);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, FString::Printf(TEXT("armlength: %f"), SpringArmComponent->TargetArmLength));

		FloatAsString = FString::SanitizeFloat(SpringArmComponent->TargetOffset.Z);
		DebugMessage = FString::Printf(TEXT("MyFloatValue: %s"), *FloatAsString);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, FString::Printf(TEXT("offsetZ: %f"), SpringArmComponent->TargetOffset.Z));

		float Angle;
		if (SpringArmComponent->TargetArmLength == 0) {
			Angle = -90;
		}
		else {
			Angle = FMath::RadiansToDegrees(atan(SpringArmComponent->TargetOffset.Z / SpringArmComponent->TargetArmLength));
			Angle = FMath::Clamp(Angle, -90, 0);
		}

		if (Angle < -90) {
			Angle = -90;
		}
		if (Angle > 0) {
			Angle = 0;
		}

		FloatAsString = FString::SanitizeFloat(Angle);
		DebugMessage = FString::Printf(TEXT("MyFloatValue: %s"), *FloatAsString);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, FString::Printf(TEXT("Angle: %f"), Angle));

		CameraComponent->SetRelativeRotation(FRotator(Angle, 0, 0));
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("ZoomError"));
	}
}

// Called every frame
void ATopDownPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


