// Fill out your copyright notice in the Description page of Project Settings.

#include "VibeCodedHenry.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

// Sets default values for this component's properties
UVibeCodedHenry::UVibeCodedHenry()
{
    PrimaryComponentTick.bCanEverTick = true;

    // Initialize pointers to nullptr to be explicit
    LeftArm = LeftForearm = LeftHand = nullptr;
    LeftLeg = LeftLowerLeg = LeftFoot = nullptr;
    RightArm = RightForearm = RightHand = nullptr;
    RightLeg = RightLowerLeg = RightFoot = nullptr;
    Neck = Head = nullptr;
}

// Called when the game starts
void UVibeCodedHenry::BeginPlay()
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
        UE_LOG(LogTemp, Error, TEXT("One or more body parts of VibeCodedHenry is null. Turning off his tick..."));
        SetComponentTickEnabled(false);
    }

    // Build quick lookup map (useful for dance tagging)
    BuildLimbLookup();

    // Only initialize dance if ticking is enabled
    if (IsComponentTickEnabled())
    {
        InitializeSimpleDance();
        CurrentDanceTime = 0.f;
    }
}

USceneComponent* UVibeCodedHenry::FindSceneComponentByTag(FName Tag, USceneComponent* Parent) {
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

void UVibeCodedHenry::BuildLimbLookup()
{
    LimbLookup.Empty();

    LimbLookup.Add(TEXT("LeftArm"), LeftArm);
    LimbLookup.Add(TEXT("LeftForearm"), LeftForearm);
    LimbLookup.Add(TEXT("LeftHand"), LeftHand);
    LimbLookup.Add(TEXT("LeftLeg"), LeftLeg);
    LimbLookup.Add(TEXT("LeftLowerLeg"), LeftLowerLeg);
    LimbLookup.Add(TEXT("LeftFoot"), LeftFoot);
    LimbLookup.Add(TEXT("RightArm"), RightArm);
    LimbLookup.Add(TEXT("RightForearm"), RightForearm);
    LimbLookup.Add(TEXT("RightHand"), RightHand);
    LimbLookup.Add(TEXT("RightLeg"), RightLeg);
    LimbLookup.Add(TEXT("RightLowerLeg"), RightLowerLeg);
    LimbLookup.Add(TEXT("RightFoot"), RightFoot);
    LimbLookup.Add(TEXT("Neck"), Neck);
    LimbLookup.Add(TEXT("Head"), Head);
}

// Replace your existing InitializeSimpleDance() with this improved version
void UVibeCodedHenry::InitializeSimpleDance()
{
    DanceAnimations.Empty();

    // Make the dance a little snappier globally
    DanceSpeed = 10.0f;

    // Helper to produce a keyframe from Euler degrees (Pitch, Yaw, Roll)
    auto KF = [](float Time, float Pitch, float Yaw, float Roll) -> FKeyframe {
        FKeyframe K;
        K.Time = Time;
        K.Rotation = FRotator(Pitch, Yaw, Roll).Quaternion();
        return K;
    };

    // Target loop duration (not strictly required; GetDuration will compute)
    const float LoopDur = 2.4f;

    // Left arm: exaggerated swing with elbow/forearm follow-through
    FLimbAnimation LeftArmAnim;
    LeftArmAnim.Keyframes = {
        KF(0.00f,   0.f,   0.f,   0.f),
        KF(0.18f, -15.f,  25.f,  18.f),
        KF(0.36f, -35.f,  70.f, -12.f),
        KF(0.60f, -20.f,  40.f,  10.f),
        KF(0.90f, -10.f,  15.f,   6.f),
        KF(1.20f,  -5.f, -10.f,   0.f),
        KF(1.56f, -30.f,  60.f, -15.f),
        KF(1.92f, -36.f,  72.f,  -6.f),
        KF(2.16f, -18.f,  28.f,  10.f),
        KF(2.40f,   0.f,   0.f,   0.f)
    };

    // Right arm: mirror of left (negate yaw and roll for a quick approximation)
    FLimbAnimation RightArmAnim = LeftArmAnim;
    for (FKeyframe& K : RightArmAnim.Keyframes)
    {
        FRotator R = K.Rotation.Rotator();
        R.Yaw = -R.Yaw;
        R.Roll = -R.Roll;
        K.Rotation = R.Quaternion();
    }

    // Forearms: small elbow bends timed slightly after the arm peaks
    FLimbAnimation LeftForearmAnim;
    LeftForearmAnim.Keyframes = {
        KF(0.00f, 0.f, 0.f, 0.f),
        KF(0.28f, 10.f, 0.f, 5.f),
        KF(0.56f, 18.f, 0.f,-10.f),
        KF(0.92f, 12.f, 0.f, 4.f),
        KF(1.30f, 6.f,  0.f,-2.f),
        KF(1.80f, 16.f, 0.f,-8.f),
        KF(2.40f, 0.f,  0.f, 0.f)
    };
    FLimbAnimation RightForearmAnim = LeftForearmAnim;

    // Hands: small flicks and twists to add flavor
    FLimbAnimation LeftHandAnim;
    LeftHandAnim.Keyframes = {
        KF(0.00f, 0.f, 0.f,  0.f),
        KF(0.20f, 6.f,  20.f, 10.f),
        KF(0.44f, 12.f, 45.f,-20.f),
        KF(0.72f, 8.f,  22.f, 12.f),
        KF(1.04f, 3.f, -10.f,  0.f),
        KF(1.44f,-6.f, -30.f, 10.f),
        KF(1.80f, 4.f,  10.f, -6.f),
        KF(2.40f, 0.f,   0.f,  0.f)
    };
    FLimbAnimation RightHandAnim = LeftHandAnim;
    for (FKeyframe& K : RightHandAnim.Keyframes)
    {
        FRotator R = K.Rotation.Rotator();
        R.Yaw = -R.Yaw;
        R.Roll = -R.Roll;
        K.Rotation = R.Quaternion();
    }

    // Torso: subtle pitch + yaw twist to sell the groove
    FLimbAnimation TorsoAnim;
    TorsoAnim.Keyframes = {
        KF(0.00f,  0.f,   0.f, 0.f),
        KF(0.30f,  1.8f, 10.f, 0.f),
        KF(0.60f,  0.5f,  4.f, 0.f),
        KF(0.96f, -1.2f,-12.f, 0.f),
        KF(1.40f,  0.8f,  6.f, 0.f),
        KF(1.80f,  0.0f,  0.f, 0.f),
        KF(2.40f,  0.f,   0.f, 0.f)
    };

    // Neck + Head: nod and slight look-left/right
    FLimbAnimation NeckAnim;
    NeckAnim.Keyframes = {
        KF(0.00f,  0.f,  0.f,  0.f),
        KF(0.36f,  5.f, 10.f,  0.f),
        KF(0.72f,  0.f,  0.f,  0.f),
        KF(1.08f, -6.f,-14.f,  0.f),
        KF(1.44f,  0.f,  4.f,  0.f),
        KF(1.92f,  3.f,  6.f,  0.f),
        KF(2.40f,  0.f,  0.f,  0.f)
    };
    FLimbAnimation HeadAnim = NeckAnim;

    // Legs: alternating small forward/back rotation to simulate shuffle
    FLimbAnimation LeftLegAnim;
    LeftLegAnim.Keyframes = {
        KF(0.00f, 0.f,  0.f, 0.f),
        KF(0.30f, 8.f,  6.f, 0.f),
        KF(0.66f, 2.f, -4.f, 0.f),
        KF(1.02f,-6.f,-10.f, 0.f),
        KF(1.44f, 4.f,  8.f, 0.f),
        KF(1.80f, 0.f,  0.f, 0.f),
        KF(2.40f, 0.f,  0.f, 0.f)
    };
    FLimbAnimation RightLegAnim = LeftLegAnim;
    // offset the timing for the right leg so they alternate
    for (FKeyframe& K : RightLegAnim.Keyframes) { K.Time = FMath::Fmod(K.Time + 0.6f, LoopDur); }
    // ensure keyframes are strictly increasing again by sorting
    RightLegAnim.Keyframes.Sort([](const FKeyframe& A, const FKeyframe& B){ return A.Time < B.Time; });
    // make sure last frame lands at LoopDur
    if (RightLegAnim.Keyframes.Num() > 0) RightLegAnim.Keyframes.Last().Time = LoopDur;

    // Feet: small rolls to sell footwork
    FLimbAnimation LeftFootAnim;
    LeftFootAnim.Keyframes = {
        KF(0.00f, 0.f, 0.f, 0.f),
        KF(0.30f, 0.f, 4.f, 6.f),
        KF(0.66f, 0.f,-6.f,-8.f),
        KF(1.08f, 0.f, 3.f, 5.f),
        KF(1.80f, 0.f, 0.f, 0.f),
        KF(2.40f, 0.f, 0.f, 0.f)
    };
    FLimbAnimation RightFootAnim = LeftFootAnim;
    for (FKeyframe& K : RightFootAnim.Keyframes) { K.Time = FMath::Fmod(K.Time + 0.6f, LoopDur); }
    RightFootAnim.Keyframes.Sort([](const FKeyframe& A, const FKeyframe& B){ return A.Time < B.Time; });
    if (RightFootAnim.Keyframes.Num() > 0) RightFootAnim.Keyframes.Last().Time = LoopDur;

    // Add animations to the map (keys must match the tags you use on actors/components)
    DanceAnimations.Add(TEXT("LeftArm"), MoveTemp(LeftArmAnim));
    DanceAnimations.Add(TEXT("RightArm"), MoveTemp(RightArmAnim));
    DanceAnimations.Add(TEXT("LeftForearm"), MoveTemp(LeftForearmAnim));
    DanceAnimations.Add(TEXT("RightForearm"), MoveTemp(RightForearmAnim));
    DanceAnimations.Add(TEXT("LeftHand"), MoveTemp(LeftHandAnim));
    DanceAnimations.Add(TEXT("RightHand"), MoveTemp(RightHandAnim));
    DanceAnimations.Add(TEXT("Torso"), MoveTemp(TorsoAnim));
    DanceAnimations.Add(TEXT("Neck"), MoveTemp(NeckAnim));
    DanceAnimations.Add(TEXT("Head"), MoveTemp(HeadAnim));
    DanceAnimations.Add(TEXT("LeftLeg"), MoveTemp(LeftLegAnim));
    DanceAnimations.Add(TEXT("RightLeg"), MoveTemp(RightLegAnim));
    DanceAnimations.Add(TEXT("LeftFoot"), MoveTemp(LeftFootAnim));
    DanceAnimations.Add(TEXT("RightFoot"), MoveTemp(RightFootAnim));

    // Compute overall dance duration (max keyframe time among animations)
    DanceDuration = 0.f;
    for (const auto& Pair : DanceAnimations)
    {
        DanceDuration = FMath::Max(DanceDuration, Pair.Value.GetDuration());
    }

    // Defensive fallback; normally DanceDuration will be the max of the keyframe lists
    if (DanceDuration <= KINDA_SMALL_NUMBER)
    {
        DanceDuration = LoopDur;
    }

    // Reset time so animation starts from the beginning
    CurrentDanceTime = 0.f;
}

// Apply dance animations each tick
void UVibeCodedHenry::UpdateDance(float DeltaTime)
{
    if (!bPlayDance || DanceAnimations.Num() == 0) return;

    CurrentDanceTime += DeltaTime;

    // Ensure dance duration is valid
    if (DanceDuration <= KINDA_SMALL_NUMBER)
    {
        // recompute defensively
        DanceDuration = 0.f;
        for (const auto& Pair : DanceAnimations)
        {
            DanceDuration = FMath::Max(DanceDuration, Pair.Value.GetDuration());
        }
        if (DanceDuration <= KINDA_SMALL_NUMBER) return;
    }

    // For each limb animation, find the component and apply slerped rotation
    for (const auto& Pair : DanceAnimations)
    {
        const FName& LimbTag = Pair.Key;
        const FLimbAnimation& Anim = Pair.Value;

        USceneComponent* Comp = nullptr;

        if (LimbLookup.Contains(LimbTag))
        {
            Comp = LimbLookup[LimbTag];
        }
        else
        {
            // fallback: common names
            if (LimbTag == TEXT("Torso"))
            {
                if (GetOwner()) Comp = GetOwner()->GetRootComponent();
            }
            else if (LimbTag == TEXT("LeftArm")) Comp = LeftArm;
            else if (LimbTag == TEXT("RightArm")) Comp = RightArm;
            else if (LimbTag == TEXT("Neck")) Comp = Neck;
        }

        if (!Comp) continue;

        FQuat TargetQ = Anim.GetRotationAtTime(CurrentDanceTime, true);

        FQuat CurQ = Comp->GetRelativeRotation().Quaternion();

        // Per-frame alpha using DanceSpeed (higher -> faster follow)
        float Alpha = FMath::Clamp(DanceSpeed * DeltaTime, 0.0f, 1.0f);

        FQuat NewQ = FQuat::Slerp(CurQ, TargetQ, Alpha).GetNormalized();

        Comp->SetRelativeRotation(NewQ.Rotator());
    }

    // Loop dance time cleanly
    if (CurrentDanceTime > DanceDuration)
    {
        CurrentDanceTime = FMath::Fmod(CurrentDanceTime, DanceDuration);
    }
}

// Called every frame
void UVibeCodedHenry::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // Update the dance system (applies quaternion-based rotations)
    UpdateDance(DeltaTime);

    // -- Optional debug block (commented out)
    /*
    for (auto* BodyPart : BodyParts) {
        if (!BodyPart) continue;
        FRotator CurrentRotation = BodyPart->GetRelativeRotation();
        UE_LOG(LogTemp, Warning, TEXT("%s rotation: Pitch=%.2f, Yaw=%.2f, Roll=%.2f"),
            *BodyPart->GetName(),
            CurrentRotation.Pitch,
            CurrentRotation.Yaw,
            CurrentRotation.Roll);
    }
    */
}
