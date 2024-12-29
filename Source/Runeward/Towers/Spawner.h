// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "GameFramework/Actor.h"
#include "Runeward/MyGameStateBase.h"
#include "Runeward/Enemeis/EnemyCharacter.h"
#include "Spawner.generated.h"

UCLASS()
class RUNEWARD_API ASpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(Replicated)
	int waveCounter;

	UPROPERTY(EditAnywhere, Category="Waves")
	int amountOfEnemiesToSpawn;

	UPROPERTY(Replicated)
	int enemiesSpawned;

	UPROPERTY(Replicated)
	float timeBetweenWaves;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere, Category="Waves")
	int WavesCooldown;

	UPROPERTY(EditAnywhere, Category="Refrecens")
	class ABulletPool* pool;

	UPROPERTY(EditAnywhere, Category="Refrecens")
	class ARunewardCharacter* player;

	UPROPERTY(EditAnywhere, Category="Refrecens")
	AMyGameStateBase* GameState;

	UPROPERTY(EditAnywhere, Category="Waves")
	float warningStop;

	UPROPERTY(Replicated)
	bool stop;

	UPROPERTY()
	FTimerHandle timer;

	UPROPERTY(EditAnywhere, Category="Waves")
	FVector spawnLocation;

	UPROPERTY(Replicated)
	bool finishedWave;

	UPROPERTY(EditAnywhere, Category="Waves")
	TArray <AEnemyCharacter*> ListOfEnemiesSpawned;

	
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void StartWave();

	UFUNCTION()
	void FinishWave(AEnemyCharacter* enemyToRemove);

	UFUNCTION()
	void SpawnEnemies();

	UFUNCTION()
	void IntervalBetweenWaves();

	UFUNCTION(BlueprintPure, Category = Functions)
	int ReturnWaveCounter();

	UFUNCTION(BlueprintPure, Category = Functions)
	FString ReturnWaveState();

};
