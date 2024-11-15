// Fill out your copyright notice in the Description page of Project Settings.


#include "Spawner.h"
#include "Kismet/GameplayStatics.h"
#include "BulletPool.h"
#include "PoolSpawnable.h"

// Sets default values
ASpawner::ASpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	waveCounter = 0;
	pool = nullptr;
	finishedWave = true;
	timeBetweenWaves = 11;
	amountOfEnemiesToSpawn = 10;
	enemiesSpawned = 0;
	WavesCooldown = 11;
	stop = false;

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

	if(finishedWave == true)
	{
		timeBetweenWaves -= DeltaTime;
	}
	
	if(finishedWave == false && stop == false)
	{
		warningStop = warningStop +	DeltaTime;	
	}
	

	if(warningStop >= 2)
	{
		stop = true;
	}

	IntervalBetweenWaves();
}

void ASpawner::IntervalBetweenWaves()
{
	if(timeBetweenWaves <= 0)
	{
		timeBetweenWaves = WavesCooldown;
		finishedWave = false;
		StartWave();
	}
}

int ASpawner::ReturnWaveCounter()
{
	return waveCounter;
}

FString ASpawner::ReturnWaveState()
{
	if(finishedWave == true)
	{
		return FString::FromInt(timeBetweenWaves);
	}
	else if(finishedWave == false && stop == false)
	{
		return "Wave Started";
	}
	else if(finishedWave == false && stop == true)
	{
		return "";
	}
	return "";
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

	enemy->SetActorLocation(GetActorLocation());

	if(IPoolSpawnable* Spawnable = Cast<IPoolSpawnable>(enemy))
	{
		Spawnable->OnSpawnedFromPool(this);
	}

	if(enemiesSpawned >= amountOfEnemiesToSpawn)
	{
		stop = false;
		enemiesSpawned = 0;	
		finishedWave = true;
		GetWorldTimerManager().ClearTimer(timer);
	}
}

