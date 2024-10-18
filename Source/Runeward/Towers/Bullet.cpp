// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "TowerBaseClass.h"
#include "GameFramework/Actor.h"

// Sets default values
ABullet::ABullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TowerMesh"));
	RootComponent = BulletMesh;
	
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("RangeSphere"));
	Collision->SetupAttachment(RootComponent);
	
	range = 1000.0f;
	Collision->SetSphereRadius(range);
	Collision->OnComponentHit.AddDynamic(this, &ABullet::ABullet::OnBulletHit);
}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABullet::OnBulletHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if(OtherActor && OtherActor->ActorHasTag(FName("Enemy")))
	{
		SetActorLocation(StartLocation);
		OnBulletHitDelegate.Broadcast(this);
	}
}

void ABullet::OnSpawnedFromPool(AActor* Requestee)
{
	ATowerBaseClass* Tower = Cast<ATowerBaseClass>(Requestee);

	FVector direction = GetActorLocation() - Tower->GetLockedEnemy()->GetActorLocation();
	direction.Normalize();

	FVector Velocity = direction * bulletVelocity;
	BulletMesh->SetPhysicsLinearVelocity(Velocity);
	
}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

