// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"

#include "WallSpline.generated.h"

UENUM()
enum class EMeshType : uint8 {
	SplineMesh,
	StaticMesh
};

UCLASS()
class DYNAMICWALL_API AWallSpline : public AActor
{
	GENERATED_BODY()


public:	
	// Sets default values for this actor's properties
	AWallSpline();

	void GenerateSplineMeshComponents();
	void GenerateWall(FVector ClickedLocation);
	void deleteComponents();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	TArray<USplineMeshComponent*> SplineMeshComponentsArray;

public:	

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", Category = "MySplineModificaiton"))
	USceneComponent* SceneComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", Category = "MySplineModificaiton"))
	USplineComponent* SplineComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", Category = "MySplineModificaiton"))
	UStaticMesh* SplineStaticMesh;

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	int32 NoOfSplinePoints;
};
