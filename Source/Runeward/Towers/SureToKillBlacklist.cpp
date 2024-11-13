// Fill out your copyright notice in the Description page of Project Settings.


#include "SureToKillBlacklist.h"

// Sets default values
ASureToKillBlacklist::ASureToKillBlacklist()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
}

// Called when the game starts or when spawned
void ASureToKillBlacklist::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASureToKillBlacklist::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASureToKillBlacklist::InsertSureToKillEnemy(AActor* SureToKillEnemy)
{
	SureToKillBlacklist.AddUnique(SureToKillEnemy);
}

bool ASureToKillBlacklist::IsEnemySureToKill(AActor* Enemy)
{
	for (int i = 0; i < SureToKillBlacklist.Num(); i++)
	{
		if(SureToKillBlacklist[i] == Enemy)
		{
			return true;
		}
	}
	return false;
}

void ASureToKillBlacklist::RemoveSureToKillEnemy(AActor* SureToKillEnemy)
{
	if(SureToKillBlacklist.Num() <= 0 )
		return;

	for (int i = 0; i < SureToKillBlacklist.Num(); i++)
	{
		if(SureToKillBlacklist[i] == SureToKillEnemy)
		{
			SureToKillBlacklist.RemoveAt(i);
		}
	}
}

