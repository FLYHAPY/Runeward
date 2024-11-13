// Fill out your copyright notice in the Description page of Project Settings.


#include "TowerBaseClass.h"

#include "Bullet.h"
#include "BulletPool.h"
#include "MainTower.h"
#include "SureToKillBlacklist.h"
#include "PoolSpawnable.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATowerBaseClass::ATowerBaseClass()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TowerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TowerMesh"));
	RootComponent = TowerMesh;
	
	RangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("RangeSphere"));
	RangeSphere->SetupAttachment(RootComponent);
	
	range = 1000.0f;
	RangeSphere->SetSphereRadius(range);
	
	fireRate = 1.0f;

	// No locked enemy at the start
	lockedEnemy = nullptr;
	hasLockedEnemy = false;
	
	RangeSphere->OnComponentEndOverlap.AddDynamic(this, &ATowerBaseClass::OnEnemyExitRange);
	RangeSphere->OnComponentBeginOverlap.AddDynamic(this, &ATowerBaseClass::OnEnemyEnterRange);
	

	spawned = false;


	Tags.Add(FName("Bullet"));

}

// Called when the game starts or when spawned
void ATowerBaseClass::BeginPlay()
{
	Super::BeginPlay();

	pool = Cast<ABulletPool>(UGameplayStatics::GetActorOfClass(GetWorld(), ABulletPool::StaticClass()));

	sureToKillBlacklist = Cast<ASureToKillBlacklist>(UGameplayStatics::GetActorOfClass(GetWorld(), ASureToKillBlacklist::StaticClass()));

	MainTower = Cast<AMainTower>(UGameplayStatics::GetActorOfClass(GetWorld(), AMainTower::StaticClass()));
}

// Called every frame
void ATowerBaseClass::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(spawned)
	{
		timer += DeltaTime;
	
		if(lockedEnemy && timer >= fireRate)
		{
			Shoot();
		}	
	}
}

void ATowerBaseClass::OnSpawnedFromPool(AActor* Requestee)
{
	spawned = true;
	CheckForEnemies();
}

void ATowerBaseClass::Shoot()
{
	if(!pool)
	{
		return;
	}
	AActor* bullet = pool->TakeObjectOut("Bullet");

	if(!bullet)
	{
		return;
	}

	if(!lockedEnemy)
	{
		LockToAnEnemy();
	}
	
	//if there is no enemy to look
	if(!lockedEnemy)
		return;

	if(sureToKillBlacklist->IsEnemySureToKill(lockedEnemy))
	{
		LockToAnEnemy();
	}

	IsLockedEnemyInsideRadius();
	
	if(!lockedEnemy)
		return;

	/*if(AEnemyBase* EnemyLocked = Cast<AEnemyBase>(lockedEnemy))
	{
		float currentEnemyHealth = EnemyLocked->GetCurrentHealth();

		if(currentEnemyHealth - damage <= 0)
		{
			sureToKillBlacklist->InsertSureToKillEnemy(lockedEnemy);
		}
	}*/
	
		
	bullet->SetActorLocation(GetActorLocation() + FVector(0, 0, 200), false, nullptr, ETeleportType::ResetPhysics);

	if(ABullet* myBullet = Cast<ABullet>(bullet))
	{
		myBullet->SetDamage(damage);
	}

	if(IPoolSpawnable* Spawnable = Cast<IPoolSpawnable>(bullet))
	{
		Spawnable->OnSpawnedFromPool(this);
	}

	timer = 0.0f;
}

void ATowerBaseClass::LockToAnEnemy()
{
	Sort();
	for(int i = 0; i < EnemiesInRange.Num(); i++)
	{
		if(!sureToKillBlacklist->IsEnemySureToKill(EnemiesInRange[i]))
		{
			lockedEnemy = EnemiesInRange[i];
			return;
		}

		//means all enemies in range are sure to kill
		if(i >= EnemiesInRange.Num() - 1)
		{
			lockedEnemy = nullptr;
		}
	}
}

void ATowerBaseClass::IsLockedEnemyInsideRadius()
{
	TArray<AActor*> OverlappingActors;
	RangeSphere->GetOverlappingActors(OverlappingActors);

	if(!OverlappingActors.Contains(lockedEnemy))
	{
		lockedEnemy = nullptr;
	}
}

void ATowerBaseClass::CheckForEnemies()
{
	// Find all actors within the range sphere
	TArray<AActor*> OverlappingActors;
	RangeSphere->GetOverlappingActors(OverlappingActors);
	
	if (OverlappingActors.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No actors found in range"));
		return;
	}

	for (int i = 0; i < OverlappingActors.Num(); i++)
	{
		// Check if the actor is an enemy and lock it
		if (OverlappingActors[i]->ActorHasTag(FName("Enemy")))
		{
			EnemiesInRange.AddUnique(OverlappingActors[i]);
			Sort();
		}
	}

	if(EnemiesInRange.Num() >= 1)
	{
		LockToAnEnemy();
	}
}

void ATowerBaseClass::OnEnemyExitRange(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor->ActorHasTag(FName("Enemy")))
	{
		EnemiesInRange.Remove(OtherActor);
		Sort();

		if(lockedEnemy == OtherActor)
		{
			lockedEnemy = nullptr;
			LockToAnEnemy();
		}
	}
}

void ATowerBaseClass::OnEnemyEnterRange(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->ActorHasTag(FName("Enemy")))
	{
		EnemiesInRange.AddUnique(OtherActor);
		Sort();
		LockToAnEnemy();
	}
}

void ATowerBaseClass::Sort()
{
	if(EnemiesInRange.Num() <= 0)
	{
		return;
	}

	if(!MainTower)
		return;
	
	EnemiesInRange.Sort([this](const AActor& A, const AActor& B)
	{
		// Get the distance between ReferenceActor and Actor A
		float DistanceA = FVector::Dist(MainTower->GetActorLocation(), A.GetActorLocation());
        
		// Get the distance between ReferenceActor and Actor B
		float DistanceB = FVector::Dist(MainTower->GetActorLocation(), B.GetActorLocation());

		// Sort in ascending order (closer first)
		return DistanceA < DistanceB;
		
	});
}

AActor* ATowerBaseClass::GetLockedEnemy()
{
	return lockedEnemy;
}

