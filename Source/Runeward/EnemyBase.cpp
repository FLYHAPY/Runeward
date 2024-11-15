// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase.h"

#include "Kismet/GameplayStatics.h"
#include "Towers/Bullet.h"
#include "Towers/BulletPool.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	EnemyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TowerMesh"));
	RootComponent = EnemyMesh;

	Tags.Add(FName("Enemy"));

	Health = 10;
	maxHeath = 10;
}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
	ScriptDelegate.BindUFunction(this, "OnBulletHit");

	if(ScriptDelegate.IsBound())
	{
		EnemyMesh->OnComponentHit.Add(ScriptDelegate);
	}

	pool = Cast<ABulletPool>(UGameplayStatics::GetActorOfClass(GetWorld(), ABulletPool::StaticClass()));
}

// Called every frame
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	float MovementForce = 100.0f;

	// Apply force in the forward direction
	FVector Force = GetActorForwardVector() * MovementForce;
	EnemyMesh->SetPhysicsLinearVelocity(Force);
}

void AEnemyBase::OnBulletHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if(OtherActor && OtherActor->ActorHasTag(FName("Bullet")))
	{
		if(ABullet* myBullet = Cast<ABullet>(OtherActor))
		{
			SetCurrentHealth(myBullet->GetDamage());
		}
	}
}

float AEnemyBase::GetCurrentHealth()
{
	return Health; 
}

void AEnemyBase::SetCurrentHealth(float towerDamage)
{
	Health -= towerDamage;

	if(Health <= 0)
	{
		SetActorLocation(FVector(0, 0, 0));
		pool->PutObjectBack("Enemy", this);
		UnregisterFromCollision();
	}
}

void AEnemyBase::OnSpawnedFromPool(AActor* Requestee)
{
	RegisterToCollision();
	Health = maxHeath;
}

void AEnemyBase::RegisterToCollision() const
{
	if(ScriptDelegate.IsBound())
	{
		EnemyMesh->OnComponentHit.AddUnique(ScriptDelegate);

		//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, BulletMesh->OnComponentHit.Contains(ScriptDelegate) ? "true" : "false");
	}
}

void AEnemyBase::UnregisterFromCollision() const
{
	if(ScriptDelegate.IsBound())
	{
		EnemyMesh->OnComponentHit.Remove(ScriptDelegate);

		//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, BulletMesh->OnComponentHit.Contains(ScriptDelegate) ? "true" : "false");
	}
}


