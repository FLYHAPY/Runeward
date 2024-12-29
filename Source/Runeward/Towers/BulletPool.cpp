// Fill out your copyright notice in the Description page of Project Settings.
#include "BulletPool.h"

#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ABulletPool::ABulletPool()
{
	Tags.Add(FName("Pool"));
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABulletPool::BeginPlay()
{
	Super::BeginPlay();

	if(UKismetSystemLibrary::IsServer(GetWorld()))
		CreatePooledObjects();
	
	//CacheBulletsIntoQueue();
}

// Called every frame
void ABulletPool::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//GEngine->AddOnScreenDebugMessage(1, 0, FColor::Red, "TakingOutCounter = " + FString::FromInt(TakingOutCounter) );
	//GEngine->AddOnScreenDebugMessage(2, 0, FColor::Green, "PuttingBack = " + FString::FromInt(PuttingBackCounter) );
}


void ABulletPool::CreatePooledObjects()
{
	for(int i = 0; i < ObjectsToSpawn.Num(); ++i)
	{
		TArray <TWeakObjectPtr<AActor>>& ActorToPopulate = MultiMap.Emplace(ObjectsToSpawn[i].Key);
		for(int j = 0; j < ObjectsToSpawn[i].howManyObjectsToSpawn; ++j)
		{
			FActorSpawnParameters spawnParameters;
			spawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			AActor* ExampleActor = GetWorld()->SpawnActor(ObjectsToSpawn[i].ClassToSpawn, &FVector::ZeroVector, &FRotator::ZeroRotator, spawnParameters);

			if(ExampleActor)
			{
				ActorToPopulate.Add(ExampleActor);
			}
		}
	}
}

//this happens when an object call the pool to teleport an object to a certain location
AActor* ABulletPool::TakeObjectOut(FName objectToTakeOut)
{
	TArray <TWeakObjectPtr<AActor>>* objectsOfThatType = MultiMap.Find(objectToTakeOut);
	if(!objectsOfThatType)
		return nullptr;
	
	int IndexToRemove = objectsOfThatType->Num() - 1;
	if(objectsOfThatType->IsValidIndex(IndexToRemove))
	{
			TWeakObjectPtr<AActor> objectPulledOut = (*objectsOfThatType)[IndexToRemove];
        	objectsOfThatType->RemoveAt(IndexToRemove);
		
        	return objectPulledOut.Get();
	}
	return nullptr;
}

//this happens when an object call the pool to be putted back into it
void ABulletPool::PutObjectBack(FName objectToTakeOut, TWeakObjectPtr<AActor> objectPulledOut)
{
	TArray <TWeakObjectPtr<AActor>>* objectsOfThatType = MultiMap.Find(objectToTakeOut);

	if(objectsOfThatType->Num() == 0)
	{
		return;
	}

	if(!objectsOfThatType)
		return;
	
	objectsOfThatType->AddUnique(objectPulledOut);
}

//Spawning all bullets and caching them into the queue
/*void ABulletPool::CacheBulletsIntoQueue()
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


void ABulletPool::TakeBulletFromQueue(ABullet* BullToTakeOff)
{
	for (int i = ActiveBullets.Num() - 1; i >= 0; --i)
	{
		if(ActiveBullets[i] == BullToTakeOff)
		{
			TWeakObjectPtr<AActor> ObjectToMove = ActiveBullets[i];
			BullToTakeOff->DisableComponentsSimulatePhysics();
			
			ActiveBullets.RemoveAt(i);
			AllBullets.Enqueue(ObjectToMove);
			
			return;
		}
	}
}

//call this in the TowerBaseClass when shoot
void ABulletPool::PortBull(AActor* Tower)
{
	TWeakObjectPtr<AActor> bullet = nullptr; 
	AllBullets.Dequeue(bullet);
	
	if(!bullet.IsValid())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "Cannot Get A Bullet From The Pool! :(");
		return;
	}
	
	ActiveBullets.Add(bullet);
	bullet->SetActorLocation(Tower->GetActorLocation() + FVector(0, 0, 200), false, nullptr, ETeleportType::ResetPhysics);

	// CPP based IMplemnentation ONly
	// Assuming we are dequeing interfaces not actors
	// Simulating the dequeued interface
	if(IPoolSpawnable* Spawnable = Cast<IPoolSpawnable>(bullet))
	{
		Spawnable->OnSpawnedFromPool(Tower);
	}
}*/
