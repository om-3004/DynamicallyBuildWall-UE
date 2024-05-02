// Fill out your copyright notice in the Description page of Project Settings.


#include "WallSpline.h"

// Sets default values
AWallSpline::AWallSpline()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SceneComponent = CreateDefaultSubobject<USceneComponent>("SceneComponent");
	RootComponent = SceneComponent;

	SplineComponent = CreateDefaultSubobject<USplineComponent>("SplineComponent");
	SplineComponent->SetupAttachment(SceneComponent);

	SplineComponent->ClearSplinePoints();
}

void AWallSpline::GenerateSplineMeshComponents()
{
	int32 NoOfSplinePoints = SplineComponent->GetNumberOfSplinePoints();

	UE_LOG(LogTemp, Warning, TEXT("No of Spline Points: %d"), NoOfSplinePoints);

	for (int i = 0; i < NoOfSplinePoints; i++)
	{
		if (i < NoOfSplinePoints - 1) {
			FVector StartPosition, StartTangent;
			SplineComponent->SetSplinePointType(i, ESplinePointType::Linear);
			SplineComponent->GetLocationAndTangentAtSplinePoint(i, StartPosition, StartTangent, ESplineCoordinateSpace::Local);

			FVector EndPosition, EndTangent;
			SplineComponent->GetLocationAndTangentAtSplinePoint(i + 1, EndPosition, EndTangent, ESplineCoordinateSpace::Local);

			USplineMeshComponent* SplineMeshComponent = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass());
			SplineMeshComponent->SetMobility(EComponentMobility::Movable);
			SplineMeshComponent->RegisterComponent();
			SplineMeshComponent->SetStaticMesh(SplineStaticMesh);

			SplineMeshComponent->SetStartAndEnd(StartPosition, StartTangent, EndPosition, EndTangent);
			SplineMeshComponent->AttachToComponent(SplineComponent, FAttachmentTransformRules::KeepRelativeTransform);
		}
	}
}
void AWallSpline::GenerateWall(FVector ClickedLocation)
{
	if (SplineComponent) {
		SplineComponent->AddSplinePoint(ClickedLocation, ESplineCoordinateSpace::Local);
		UE_LOG(LogTemp, Warning, TEXT("Hit Location: %s"), *ClickedLocation.ToString());

		GenerateSplineMeshComponents();
	}
}

// Called when the game starts or when spawned
void AWallSpline::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWallSpline::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

}

// Called every frame
void AWallSpline::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

