// Fill out your copyright notice in the Description page of Project Settings.


#include "Spawner.h"
#include "Kismet/GameplayStatics.h"
#include "BulletPool.h"

// Sets default values
ASpawner::ASpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	waveCounter = 0;
	pool = nullptr;
	finishedWave = true;
	timeBetweenWaves = 0;
	amountOfEnemiesToSpawn = 10;
	enemiesSpawned = 0;
	WavesCooldown = 10;

}

// Called when the game starts or when spawned
void ASpawner::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> FoundPool;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Pool"), FoundPool);

	if(FoundPool.Num() >= 1)
	{
		pool = Cast<ABulletPool>(FoundPool[0]);
	}
}

// Called every frame
void ASpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	GEngine->AddOnScreenDebugMessage(1, 0, FColor::Red, "Time Between Waves = " + FString::SanitizeFloat(timeBetweenWaves, 2));

	if(finishedWave == true)
	{
		timeBetweenWaves += DeltaTime;
	}
	IntervalBetweenWaves(DeltaTime);
}

void ASpawner::IntervalBetweenWaves(float DeltaTime)
{
	if(finishedWave == true)
	{
		timeBetweenWaves += DeltaTime;
	}

	if(timeBetweenWaves >= WavesCooldown)
	{
		timeBetweenWaves = 0;
		finishedWave = false;
		StartWave();
	}
}

void ASpawner::StartWave()
{
	waveCounter ++;
	GetWorldTimerManager().SetTimer(timer, this, &ASpawner::SpawnEnemies, 1.0f, true, 2.0f);
}

void ASpawner::SpawnEnemies()
{
	enemiesSpawned++;
	AActor* enemy = pool->TakeObjectOut("Enemy");

	if(!enemy)
	{
		return;
	}

	enemy->SetActorLocation(spawnLocation);

	if(enemiesSpawned >= amountOfEnemiesToSpawn)
	{
		enemiesSpawned = 0;	
		finishedWave = true;
		GetWorldTimerManager().ClearTimer(timer);
	}
}

