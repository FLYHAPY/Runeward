// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase.h"
#include "Towers/BulletPool.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	EnemyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TowerMesh"));
	RootComponent = EnemyMesh;

	Tags.Add(FName("Enemy"));
}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	FScriptDelegate ScriptDelegate;
	ScriptDelegate.BindUFunction(this, "OnBulletHit");

	if(ScriptDelegate.IsBound())
	{
		EnemyMesh->OnComponentHit.Add(ScriptDelegate);
	}
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
		pool->PutObjectBack("Enemy", this);
		SetActorLocation(FVector(0, 0, 0));
	}
}

