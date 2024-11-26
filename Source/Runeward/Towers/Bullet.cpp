// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"

#include "BulletPool.h"
#include "Components/StaticMeshComponent.h"
#include "TowerBaseClass.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Runeward/Enemeis/EnemyCharacter.h"

// Sets default values
ABullet::ABullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TowerMesh"));
	RootComponent = BulletMesh;

	BulletMesh->BodyInstance.bNotifyRigidBodyCollision = true;
	BulletMesh->BodyInstance.SetEnableGravity(false);
	
	//Collision = CreateDefaultSubobject<USphereComponent>(TEXT("RangeSphere"));
	//Collision->SetupAttachment(RootComponent);
	
	//range = 1000.0f;
	//Collision->SetSphereRadius(range);

	bulletSpeed = 10;
	Tags.Add(FName("Bullet"));
}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();
	
	TArray<AActor*> FoundPool;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Pool"), FoundPool);

	if(FoundPool.Num() >= 1)
	{
		pool = Cast<ABulletPool>(FoundPool[0]);
	}
	
	ScriptDelegate.BindUFunction(this, "OnBulletHit");

}

void ABullet::OnBulletHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	pool->PutObjectBack("Bullet", this);
	BulletMesh->BodyInstance.SetInstanceSimulatePhysics(false);
	BulletMesh->ComponentVelocity = FVector::ZeroVector;
	UnregisterFromCollision();
	SetActorLocation(StartLocation);

	if(AEnemyCharacter* enemy = Cast<AEnemyCharacter>(OtherActor))
	{
		enemy->SetCurrentHealth(damage);
	}
	
}

void ABullet::OnSpawnedFromPool(AActor* Requestee)
{
	if(ATowerBaseClass* Tower = Cast<ATowerBaseClass>(Requestee))
	{
		BulletMesh->BodyInstance.SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		FVector BulletPosition = GetActorLocation();
		FVector EnemyPosition = Tower->GetLockedEnemy()->GetActorLocation() + FVector(0, 0, 50);
		FVector EnemyVelocity = Tower->GetLockedEnemy()->GetVelocity();

		FVector DeltaPosition = EnemyPosition - BulletPosition;

		
		float a = FVector::DotProduct(EnemyVelocity, EnemyVelocity) - bulletSpeed * bulletSpeed;
		float b = 2.0f * FVector::DotProduct(DeltaPosition, EnemyVelocity);
		float c = FVector::DotProduct(DeltaPosition, DeltaPosition);
		
		float Discriminant = b * b - 4.0f * a * c;
		if (Discriminant < 0) {
			// No real solution, bullet can't reach the target
			return;
		}

		float SqrtDiscriminant = FMath::Sqrt(Discriminant);
		float t1 = (-b + SqrtDiscriminant) / (2.0f * a);
		float t2 = (-b - SqrtDiscriminant) / (2.0f * a);

		float TimeToHit = (t1 > 0 && (t1 < t2 || t2 <= 0)) ? t1 : (t2 > 0 ? t2 : -1.0f);
		if (TimeToHit < 0) {
			// No positive solution, bullet can't reach the target
			return;
		}

		FVector PredictedEnemyPosition = EnemyPosition + EnemyVelocity * TimeToHit;

		FVector ShootingDirection = (PredictedEnemyPosition - BulletPosition).GetSafeNormal();

		FVector Velocity = ShootingDirection * bulletSpeed;
		BulletMesh->SetSimulatePhysics(true);
		BulletMesh->SetPhysicsLinearVelocity(Velocity);
		
		/*FVector direction = Tower->GetLockedEnemy()->GetActorLocation() - GetActorLocation();
		direction.Normalize();
        
		FVector Velocity = direction * bulletVelocity;

		BulletMesh->SetSimulatePhysics(true);
		BulletMesh->SetPhysicsLinearVelocity(Velocity);*/

		RegisterToCollision();
	}
}

void ABullet::RegisterToCollision() const
{
	if(ScriptDelegate.IsBound())
	{
		BulletMesh->OnComponentHit.AddUnique(ScriptDelegate);

		//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, BulletMesh->OnComponentHit.Contains(ScriptDelegate) ? "true" : "false");
	}
}

void ABullet::UnregisterFromCollision() const
{
	if(ScriptDelegate.IsBound())
	{
		BulletMesh->OnComponentHit.Remove(ScriptDelegate);

		//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, BulletMesh->OnComponentHit.Contains(ScriptDelegate) ? "true" : "false");
	}
}

void ABullet::SetDamage(float TowerDamage)
{
	damage = TowerDamage;
}

float ABullet::GetDamage()
{
	return damage;
}

float ABullet::GetBulletSpeed()
{
	return bulletSpeed;
}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);	
}


