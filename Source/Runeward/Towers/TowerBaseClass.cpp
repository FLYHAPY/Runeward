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
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ATowerBaseClass::ATowerBaseClass()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TowerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TowerMesh"));
	RootComponent = TowerMesh;

	firePosition = CreateDefaultSubobject<USceneComponent>(TEXT("MySceneComponent"));
	firePosition->SetupAttachment(TowerMesh);
	
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

	myBulletSpeed = 2000;


	Tags.Add(FName("Bullet"));

	timer = 0;

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
		if(lockedEnemy)
		{
			timer += DeltaTime;

			if(timer >= fireRate)
			{
				Shoot();
			}
		}	
	}

	if(lockedEnemy)
	{
		RotateCannonTowardsEnemy(myBulletSpeed);
	}
}

void ATowerBaseClass::OnSpawnedFromPool(AActor* Requestee)
{
	spawned = true;
	CheckForEnemies();
}

float ATowerBaseClass::TowerCost()
{
	return cost;
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
	
		
	bullet->SetActorLocation(firePosition->GetComponentLocation(), false, nullptr, ETeleportType::ResetPhysics);

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
	if(EnemiesInRange.Num() > 0)
		lockedEnemy = EnemiesInRange[0];
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


void ATowerBaseClass::RotateCannonTowardsEnemy(float BulletSpeed)
{
	
	// Calculate the predicted position
	FVector PredictedPosition = PredictEnemyPosition(BulletSpeed);

	FRotator DesiredRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PredictedPosition);

	// Constrain rotation to specific axes if needed (e.g., only Yaw)
	DesiredRotation.Pitch = FMath::Clamp(0, -10.0f, 45.0f); // Example: Clamp pitch for cannon elevation

	// Adjust for initial offset
	FRotator InitialRotationOffset = FRotator(0, 90.0f, 0); // Adjust this based on the initial setup
	DesiredRotation -= InitialRotationOffset;

	// Interpolate and apply
	FRotator NewRotation = FMath::RInterpTo(TowerMesh->GetRelativeRotation(), DesiredRotation, GetWorld()->DeltaTimeSeconds, 5.0f);
	TowerMesh->SetRelativeRotation(NewRotation);
}

FVector ATowerBaseClass::PredictEnemyPosition(float BulletSpeed)
{
	FVector DirectionToEnemy = lockedEnemy->GetActorLocation() - GetActorLocation();
	float DistanceToEnemy = DirectionToEnemy.Size();
	float TimeToImpact = DistanceToEnemy / BulletSpeed;

	// Predict the enemy's position based on its velocity and time to impact
	FVector PredictedPosition = lockedEnemy->GetActorLocation() + (lockedEnemy->GetVelocity() * TimeToImpact);
	return PredictedPosition;
}

