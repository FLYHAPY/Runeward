// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameStateBase.h"

#include "Net/UnrealNetwork.h"

void AMyGameStateBase::Server_AddCoins_Implementation(float coinsToGet)
{
	coins += coinsToGet;
}

void AMyGameStateBase::RemoveCoins(float coinsToRemove)
{
	coins -= coinsToRemove;
}

void AMyGameStateBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyGameStateBase, coins);
}

float AMyGameStateBase::GetCoins() const
{
	return coins;
}
