// Fill out your copyright notice in the Description page of Project Settings.


#include "Henry.h"

// Sets default values for this component's properties
UHenry::UHenry()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHenry::BeginPlay()
{
	Super::BeginPlay();
	
	LeftArm = FindSceneComponentByTag(TEXT("LeftArm"));
	LeftForearm = FindSceneComponentByTag(TEXT("LeftForearm"), LeftArm);
	LeftHand = FindSceneComponentByTag(TEXT("LeftHand"), LeftForearm);
	LeftLeg = FindSceneComponentByTag(TEXT("LeftLeg"));
	LeftLowerLeg = FindSceneComponentByTag(TEXT("LeftLowerLeg"), LeftLeg);
	LeftFoot = FindSceneComponentByTag(TEXT("LeftFoot"), LeftLowerLeg);
	RightArm = FindSceneComponentByTag(TEXT("RightArm"));
	RightForearm = FindSceneComponentByTag(TEXT("RightForearm"), RightArm);
	RightHand = FindSceneComponentByTag(TEXT("RightHand"), RightForearm);
	RightLeg = FindSceneComponentByTag(TEXT("RightLeg"));
	RightLowerLeg = FindSceneComponentByTag(TEXT("RightLowerLeg"), RightLeg);
	RightFoot = FindSceneComponentByTag(TEXT("RightFoot"), RightLowerLeg);
	Neck = FindSceneComponentByTag(TEXT("Neck"));
	Head = FindSceneComponentByTag(TEXT("Head"), Neck);
	BodyParts = {
		LeftArm, LeftForearm, LeftHand, LeftLeg, LeftLowerLeg, LeftFoot,
		RightArm, RightForearm, RightHand, RightLeg, RightLowerLeg, RightFoot,
		Neck, Head
	};

	if (BodyParts.Contains(nullptr)) {
		UE_LOG(LogTemp, Error, TEXT("One or more body parts of Henry is null. Turning off his tick..."));
		SetComponentTickEnabled(false);
	}
}

USceneComponent* UHenry::FindSceneComponentByTag(FName Tag, USceneComponent* Parent) {
	TArray<AActor*> Children;
	if (Parent) {
		Parent->GetOwner()->GetAttachedActors(Children, true);
	}
	else {
		GetOwner()->GetAttachedActors(Children, true);
	}
	AActor** SearchedChild = Children.FindByPredicate([&Tag](AActor* Actor) {
		return Actor->ActorHasTag(Tag);
	});
	return SearchedChild ? (*SearchedChild)->GetRootComponent() : nullptr;
}


// Called every frame
void UHenry::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// for (auto* BodyPart : BodyParts) {
	// 	BodyPart->AddRelativeRotation(FRotator(0, 10, 0));
	// 	FRotator CurrentRotation = BodyPart->GetRelativeRotation();
	// 	
	// 	UE_LOG(LogTemp, Warning, TEXT("%s rotation: Pitch=%.2f, Yaw=%.2f, Roll=%.2f"),
	// 	*BodyPart->GetName(),
	// 	CurrentRotation.Pitch,
	// 	CurrentRotation.Yaw,
	// 	CurrentRotation.Roll);
	// }
	FQuat CurrentRotation = LeftArm->GetRelativeRotation().Quaternion();
	FQuat TargetRotation{30, 90, 0, 1};
	
	float Alpha = FMath::Clamp(.001 * DeltaTime, 0.0f, 1.0f);
	
	LeftArm->SetRelativeRotation(FQuat::Slerp(CurrentRotation, TargetRotation, Alpha).Rotator());
}

