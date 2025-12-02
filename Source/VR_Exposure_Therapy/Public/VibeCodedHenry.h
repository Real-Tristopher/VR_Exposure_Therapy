// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "VibeCodedHenry.generated.h"

// Simple keyframe: time in seconds + quaternion rotation
struct FKeyframe
{
    float Time = 0.f;
    FQuat Rotation = FQuat::Identity;
};

// Per-limb animation: ordered keyframes, helper to sample (with optional looping)
struct FLimbAnimation
{
    TArray<FKeyframe> Keyframes;

    float GetDuration() const
    {
        if (Keyframes.Num() == 0) return 0.f;
        return Keyframes.Last().Time;
    }

    // Samples rotation at InTime. If bLoop is true, time wraps around duration.
    FQuat GetRotationAtTime(float InTime, bool bLoop = true) const
    {
        const int32 Num = Keyframes.Num();
        if (Num == 0) return FQuat::Identity;
        if (Num == 1) return Keyframes[0].Rotation;

        float Duration = GetDuration();
        if (Duration <= KINDA_SMALL_NUMBER) return Keyframes.Last().Rotation;

        float Time = InTime;
        if (bLoop)
        {
            Time = FMath::Fmod(InTime, Duration);
            if (Time < 0.f) Time += Duration;
        }
        else
        {
            Time = FMath::Clamp(InTime, 0.f, Duration);
        }

        // Find interval [A,B] where A.Time <= Time <= B.Time
        for (int32 i = 0; i < Num - 1; ++i)
        {
            const FKeyframe& A = Keyframes[i];
            const FKeyframe& B = Keyframes[i + 1];
            if (Time >= A.Time && Time <= B.Time)
            {
                float Alpha = (Time - A.Time) / (B.Time - A.Time);
                return FQuat::Slerp(A.Rotation, B.Rotation, Alpha).GetNormalized();
            }
        }

        // If not found (time past last), return last
        return Keyframes.Last().Rotation;
    }
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VR_EXPOSURE_THERAPY_API UVibeCodedHenry : public USceneComponent
{
    GENERATED_BODY()

public:    
    // Sets default values for this component's properties
    UVibeCodedHenry();

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

    // Your provided helper (kept signature)
    virtual USceneComponent* FindSceneComponentByTag(FName Tag, USceneComponent* Parent = nullptr);

public:    
    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // Body part pointers (kept as you provided)
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

    // --- Dance system members (additions) ---
public:
    // Map tag -> limb animation
    TMap<FName, FLimbAnimation> DanceAnimations;

    // Quick lookup tag -> component
    TMap<FName, USceneComponent*> LimbLookup;

    // Current play time (seconds)
    float CurrentDanceTime = 0.f;

    // Overall dance duration (seconds)
    float DanceDuration = 0.f;

    // Whether dance plays
    bool bPlayDance = true;

    // Global speed multiplier for slerp alpha calculation
    float DanceSpeed = 8.0f;

    // Initialize a small sample dance (fills DanceAnimations)
    void InitializeSimpleDance();

    // Update dance each tick (apply rotations to components)
    void UpdateDance(float DeltaTime);

    // Optional: helper to build LimbLookup after BodyParts are set
    void BuildLimbLookup();
};
