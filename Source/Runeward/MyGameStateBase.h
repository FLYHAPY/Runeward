// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "MyGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class RUNEWARD_API AMyGameStateBase : public AGameState
{
	GENERATED_BODY()

private:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	UPROPERTY(Replicated)
	float coins = 400.0f;

public:
	UFUNCTION(Server, Reliable)
	void Server_AddCoins(float coinsToGet);

	void RemoveCoins(float coinsToRemove);

	UFUNCTION(BlueprintPure)
	float GetCoins() const;
};
