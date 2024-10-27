// Fill out your copyright notice in the Description page of Project Settings.


#include "TowerBaseClass.h"

#include "BulletPool.h"
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




}

// Called when the game starts or when spawned
void ATowerBaseClass::BeginPlay()
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
void ATowerBaseClass::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	CheckForEnemies();

	timer += DeltaTime;
	
	if(lockedEnemy && timer >= fireRate)
	{
		if(!pool)
		{
			return;
		}
		AActor* bullet = pool->TakeObjectOut("bullet");
		bullet->SetActorLocation(GetActorLocation() + FVector(0, 0, 200));
	}
}

void ATowerBaseClass::CheckForEnemies()
{
	if (hasLockedEnemy)
	{
		// If we already have a enemy locked do nothing!!
		return;
	}
	
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
			UE_LOG(LogTemp, Warning, TEXT("Found it"));
			lockedEnemy = OverlappingActors[i];
			hasLockedEnemy = true;
			UE_LOG(LogTemp, Warning, TEXT("Locked onto enemy: %s"), *lockedEnemy->GetName());
			return;
			// no need to go forward because we lock it already
		}
	}
}

AActor* ATowerBaseClass::GetLockedEnemy()
{
	return lockedEnemy;
}

void ATowerBaseClass::Shoot()
{
	timer = 0.0f;
	UE_LOG(LogTemp, Warning, TEXT("Shooting at enemy: %s"), *lockedEnemy->GetName());
}

void ATowerBaseClass::OnEnemyExitRange(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == lockedEnemy)
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemy exited range: %s"), *lockedEnemy->GetName());
		lockedEnemy = nullptr;
		hasLockedEnemy = false;
	}
}

void ATowerBaseClass::OnEnemyEnterRange(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    
	if (OtherActor && OtherActor->ActorHasTag(FName("Enemy")) && !hasLockedEnemy)
	{
		lockedEnemy = OtherActor;
		hasLockedEnemy = true;
	}
}

