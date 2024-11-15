// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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

	UPROPERTY(EditAnywhere, Category="Waves")
	int waveCounter;

	UPROPERTY(EditAnywhere, Category="Waves")
	int amountOfEnemiesToSpawn;

	UPROPERTY(EditAnywhere, Category="Waves")
	int enemiesSpawned;

	UPROPERTY(EditAnywhere, Category="Waves")
	float timeBetweenWaves;

	UPROPERTY(EditAnywhere, Category="Waves")
	int WavesCooldown;

	UPROPERTY(EditAnywhere, Category="Refrecens")
	class ABulletPool* pool;

	float warningStop;

	bool stop;

	UPROPERTY()
	FTimerHandle timer;

	UPROPERTY(EditAnywhere, Category="Waves")
	FVector spawnLocation;

	bool finishedWave;

	
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void StartWave();

	UFUNCTION()
	void SpawnEnemies();

	UFUNCTION()
	void IntervalBetweenWaves();

	UFUNCTION(BlueprintPure, Category = Functions)
	int ReturnWaveCounter();

	UFUNCTION(BlueprintPure, Category = Functions)
	FString ReturnWaveState();

};
