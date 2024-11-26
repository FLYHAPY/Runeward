// Fill out your copyright notice in the Description page of Project Settings.


#include "MakePlayerStopAttackingOnEnd.h"

#include "RunewardCharacter.h"

void UMakePlayerStopAttackingOnEnd::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp) return;
	
	if (AActor* Owner = MeshComp->GetOwner())
	{
		if (ARunewardCharacter* PlayerCharacter = Cast<ARunewardCharacter>(Owner))
		{
			PlayerCharacter->ResetAttack();
		}
	}
	
}
