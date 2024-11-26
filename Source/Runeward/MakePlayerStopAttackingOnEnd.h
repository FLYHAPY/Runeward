// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "MakePlayerStopAttackingOnEnd.generated.h"

/**
 * 
 */
UCLASS()
class RUNEWARD_API UMakePlayerStopAttackingOnEnd : public UAnimNotify
{
	GENERATED_BODY()
public:
	// Called when the notify state ends
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
