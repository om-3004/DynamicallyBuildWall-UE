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

}

void AWallSpline::GenerateSplineMeshes()
{
	for (int i=0 ; i<SplineMeshComponents.Num() ; i++)
	{
		if (SplineMeshComponents[i])
		{
			SplineMeshComponents[i]->DestroyComponent();
			SplineMeshComponents[i] = nullptr;
		}
	}
	SplineMeshComponents.Empty();
	
	for (int i=0 ; i<StaticMeshComponents.Num() ; i++)
	{
		if (StaticMeshComponents[i])
		{
			StaticMeshComponents[i]->DestroyComponent();
			StaticMeshComponents[i] = nullptr;
		}
	}
	StaticMeshComponents.Empty();

	int32 NoOfSplinePoints = SplineComponent->GetNumberOfSplinePoints();
	for (int i=0 ; i<NoOfSplinePoints ; i++)
	{
		if(i < NoOfSplinePoints - 1) {
			FVector StartPosition, StartTangent;
			SplineComponent->GetLocationAndTangentAtSplinePoint(i, StartPosition, StartTangent, ESplineCoordinateSpace::World);

			FVector EndPosition, EndTangent;
			SplineComponent->GetLocationAndTangentAtSplinePoint(i + 1, EndPosition, EndTangent, ESplineCoordinateSpace::World);

			USplineMeshComponent* SplineMeshComponent = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass());
			SplineMeshComponent->RegisterComponent();
			SplineMeshComponent->SetStaticMesh(SplineStaticMesh);

			SplineMeshComponent->SetStartAndEnd(StartPosition, StartTangent, EndPosition, EndTangent);
			SplineMeshComponent->SetupAttachment(SplineComponent);
			SplineMeshComponent->ForwardAxis = MeshForwardAxis;

			SplineMeshComponents.Add(SplineMeshComponent);
		}
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

	GenerateSplineMeshes();
}

// Called every frame
void AWallSpline::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

