// Fill out your copyright notice in the Description page of Project Settings.
#include "BulletPool.h"
#include "Bullet.h"

// Sets default values
ABulletPool::ABulletPool()
{
	Tags.Add(FName("Pool"));
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bulletsToSpawn = 10;
}

// Called when the game starts or when spawned
void ABulletPool::BeginPlay()
{
	Super::BeginPlay();
	CacheBulletsIntoQueue();
}

// Called every frame
void ABulletPool::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


//Spawning all bullets and caching them into the queu
void ABulletPool::CacheBulletsIntoQueue()
{
	UWorld* World = GetWorld();
	FRotator SpawnRotation = FRotator::ZeroRotator;
	FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
	
	for (int i = 0; i < bulletsToSpawn; i++)
	{
		ABullet* SpawnedBullet = World->SpawnActor<ABullet>(Bullet, SpawnLocation, SpawnRotation, SpawnParameters);
		
		if (SpawnedBullet)
		{
			SpawnedBullet->OnBulletHitDelegate.AddDynamic(this, &ABulletPool::TakeBulletFromQueue);
			AllBullets.Enqueue(SpawnedBullet);
		}
	}
}

//This will happen when a bullet collides with an enemy
void ABulletPool::TakeBulletFromQueue(ABullet* BullToTakeOff)
{
	for (int i = ActiveBullets.Num() - 1; i >= 0; --i)
	{
		if(ActiveBullets[i] == BullToTakeOff)
		{
			AllBullets.Enqueue(ActiveBullets[i]);
			ActiveBullets.RemoveAt(i);
		}
	}
}


//call this in the TowerBaseClass when shoot
void ABulletPool::PortBull(FVector position)
{
	TWeakObjectPtr<AActor> bullet = nullptr; 
	AllBullets.Dequeue(bullet);
	ActiveBullets.Add(bullet);
	bullet->SetActorLocation(position);

	// CPP based IMplemnentation ONly
	// Assuming we are dequeing interfaces not actors
	// Simulating the dequeued interface
	if(IPoolSpawnable* Spawnable = Cast<IPoolSpawnable>(bullet))
	{
		Spawnable->OnSpawnedFromPool(this);
	}
}
