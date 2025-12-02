// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "Components/SceneComponent.h"
#include "Henry.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VR_EXPOSURE_THERAPY_API UHenry : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHenry();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual USceneComponent* FindSceneComponentByTag(FName Tag, USceneComponent* Parent = nullptr);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	USceneComponent* LeftArm;
	USceneComponent* LeftForearm;
	USceneComponent* LeftHand;
	USceneComponent* LeftLeg;
	USceneComponent* LeftLowerLeg;
	USceneComponent* LeftFoot;
	USceneComponent* RightArm;
	USceneComponent* RightForearm;
	USceneComponent* RightHand;
	USceneComponent* RightLeg;
	USceneComponent* RightLowerLeg;
	USceneComponent* RightFoot;
	USceneComponent* Neck;
	USceneComponent* Head;
	TArray<USceneComponent*> BodyParts;
};
