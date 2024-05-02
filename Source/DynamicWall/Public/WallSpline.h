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
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", Category = "MySplineModificaiton"))
	USceneComponent* SceneComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", Category = "MySplineModificaiton"))
	USplineComponent* SplineComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", Category = "MySplineModificaiton"))
	TArray<USplineMeshComponent*> SplineMeshComponents;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", Category = "MySplineModificaiton"))
	TArray<UStaticMeshComponent*> StaticMeshComponents;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", Category = "MySplineModificaiton"))
	UStaticMesh* SplineStaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", Category = "MySplineModificaiton"))
	TEnumAsByte<ESplineMeshAxis::Type> MeshForwardAxis = ESplineMeshAxis::X;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", Category = "MySplineModificaiton"))
	TEnumAsByte<EMeshType> MeshType;


public:	
	// Sets default values for this actor's properties
	AWallSpline();

	UFUNCTION(BlueprintCallable)
	void GenerateSplineMeshes();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
