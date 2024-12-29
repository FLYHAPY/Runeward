// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "GameFramework/Actor.h"
#include "SureToKillBlacklist.generated.h"

UCLASS()
class RUNEWARD_API ASureToKillBlacklist : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASureToKillBlacklist();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	TArray<AActor*> SureToKillBlacklist;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void InsertSureToKillEnemy(AActor* SureToKillEnemy);

	bool IsEnemySureToKill(AActor* Enemy);

	void RemoveSureToKillEnemy(AActor* SureToKillEnemy);
	

};
