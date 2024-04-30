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
	SpringArmComponent->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 150.f;
	SpringArmComponent->TargetOffset = FVector(0.f, 0.f, 250.f);
	SpringArmComponent->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraComponent->SetRelativeLocation(FVector(-30.f, 0.f, 30.f));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);

	FloatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FloatingPawnMovement"));
}

void ATopDownPawn::Spawn()
{
	FVector PawnLocation = {0.f, 0.f, 100.f};
	FRotator PawnRotation = {0.f, 0.f, 0.f};
	//if (GetController()->GetPawn()) {

	//	PawnLocation = GetController()->GetPawn()->GetActorLocation();
	//	PawnRotation = GetController()->GetPawn()->GetActorRotation();

	//	//GetController()->GetPawn()->Destroy();
	//}

	UWorld* World = GetWorld();

	if (World) {
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		/*if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(World->GetFirstLocalPlayerFromController())) {
			Subsystem->ClearAllMappings();
		}*/

		APawn* SpawnedPawn = World->SpawnActor<APawn>(PawnLocation, PawnRotation, SpawnParams);

		if (SpawnedPawn) {
			APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
			if (PlayerController)
			{
				PlayerController->Possess(this);
			}
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Succeeded to Spawn"));
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Failed to Spawn"));
		}
	}
}

// Called when the game starts or when spawned
void ATopDownPawn::BeginPlay()
{
	Super::BeginPlay();

	Spawn();
}


void ATopDownPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
	//	UInputMappingContext* InputMappingContext = NewObject<UInputMappingContext>();

	//	UInputAction* MoveAction = NewObject<UInputAction>();
	//	MoveAction->ValueType = EInputActionValueType::Axis3D;

	//	UInputModifierSwizzleAxis* YXZSwizzleAxisModifier = NewObject<UInputModifierSwizzleAxis>();
	//	YXZSwizzleAxisModifier->Order = EInputAxisSwizzle::YXZ;

	//	UInputModifierSwizzleAxis* ZYXSwizzleAxisModifier = NewObject<UInputModifierSwizzleAxis>();
	//	ZYXSwizzleAxisModifier->Order = EInputAxisSwizzle::ZYX;

	//	UInputModifierNegate* MoveActionNegateModifier = NewObject<UInputModifierNegate>();
	//	MoveActionNegateModifier->bX = true;
	//	MoveActionNegateModifier->bY = true;
	//	MoveActionNegateModifier->bZ = true;

	//	InputMappingContext->MapKey(MoveAction, EKeys::W);

	//	FEnhancedActionKeyMapping& MoveActionBackwardKeyMapping = InputMappingContext->MapKey(MoveAction, EKeys::S);
	//	MoveActionBackwardKeyMapping.Modifiers.Add(MoveActionNegateModifier);

	//	FEnhancedActionKeyMapping& MoveActionRightKeyMapping = InputMappingContext->MapKey(MoveAction, EKeys::D);
	//	MoveActionRightKeyMapping.Modifiers.Add(YXZSwizzleAxisModifier);

	//	FEnhancedActionKeyMapping& MoveActionLeftKeyMapping = InputMappingContext->MapKey(MoveAction, EKeys::A);
	//	MoveActionLeftKeyMapping.Modifiers.Add(YXZSwizzleAxisModifier);
	//	MoveActionLeftKeyMapping.Modifiers.Add(MoveActionNegateModifier);

	//	FEnhancedActionKeyMapping& MoveActionUpKeyMapping = InputMappingContext->MapKey(MoveAction, EKeys::E);
	//	MoveActionUpKeyMapping.Modifiers.Add(ZYXSwizzleAxisModifier);

	//	FEnhancedActionKeyMapping& MoveActionDownKeyMapping = InputMappingContext->MapKey(MoveAction, EKeys::Q);
	//	MoveActionDownKeyMapping.Modifiers.Add(ZYXSwizzleAxisModifier);
	//	MoveActionDownKeyMapping.Modifiers.Add(MoveActionNegateModifier);


	//	UInputAction* LookAction = NewObject<UInputAction>();
	//	LookAction->ValueType = EInputActionValueType::Axis2D;

	//	UInputModifierNegate* LookActionMouseNegateModifier = NewObject<UInputModifierNegate>();
	//	LookActionMouseNegateModifier->bX = false;
	//	LookActionMouseNegateModifier->bY = true;
	//	LookActionMouseNegateModifier->bZ = false;

	//	FEnhancedActionKeyMapping& LookActionMouseMapping = InputMappingContext->MapKey(LookAction, EKeys::Mouse2D);
	//	LookActionMouseMapping.Modifiers.Add(LookActionMouseNegateModifier);

	//	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATopDownPawn::Move);
	//	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATopDownPawn::Look);

	//	if (const APlayerController* PlayerController = Cast<APlayerController>(Controller)) {
	//		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) {
	//			//Subsystem->ClearAllMappings();
	//			Subsystem->AddMappingContext(InputMappingContext, 0);
	//		}
	//	}

	//}

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

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATopDownPawn::Move);

		if (const APlayerController* PlayerController = Cast<APlayerController>(Controller)) {
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) {
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}

	}
}

//void ATopDownPawn::Look(const FInputActionValue& Value)
//{
//	FVector2D LookAxisVector = Value.Get<FVector2D>();
//
//	AddControllerYawInput(LookAxisVector.X);
//	AddControllerPitchInput(LookAxisVector.Y);
//}

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

// Called every frame
void ATopDownPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
//void ATopDownPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
//{
//	Super::SetupPlayerInputComponent(PlayerInputComponent);
//
//}


