// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include "Components/StaticMeshComponent.h"
#include "TowerBaseClass.h"
#include "GameFramework/Actor.h"

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

	bulletVelocity = 10;
}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();

	FScriptDelegate ScriptDelegate;
	ScriptDelegate.BindUFunction(this, "OnBulletHit");
    
	if(ScriptDelegate.IsBound())
	{
		BulletMesh->OnComponentHit.Add(ScriptDelegate);

		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, BulletMesh->OnComponentHit.Contains(ScriptDelegate) ? "true" : "false");
	}
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
	if(ATowerBaseClass* Tower = Cast<ATowerBaseClass>(Requestee))
	{
		FVector direction = Tower->GetLockedEnemy()->GetActorLocation() - GetActorLocation();
		direction.Normalize();
        
		FVector Velocity = direction * bulletVelocity;

		BulletMesh->SetSimulatePhysics(true);
		BulletMesh->SetPhysicsLinearVelocity(Velocity);
	}
}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
}

