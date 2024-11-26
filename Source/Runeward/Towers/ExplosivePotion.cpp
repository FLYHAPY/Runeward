// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplosivePotion.h"

#include "BulletPool.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Runeward/EnemyBase.h"
#include "Runeward/Enemeis/EnemyCharacter.h"

// Sets default values
AExplosivePotion::AExplosivePotion()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AOESphere = CreateDefaultSubobject<USphereComponent>(TEXT("AOESphere"));
	RootComponent = AOESphere;

	ExplosivePotionMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ExplosiveMesh"));
	ExplosivePotionMesh->SetupAttachment(RootComponent);
	

	int range = 1000.0f;
	AOESphere->SetSphereRadius(range);

	
	ExplosivePotionMesh->SetSimulatePhysics(false);
}

// Called when the game starts or when spawned
#pragma optimize("", off)
void AExplosivePotion::BeginPlay()
{
	Super::BeginPlay();
	
	ScriptDelegate1.BindUFunction(this, "DetectEnemy");

	pool = Cast<ABulletPool>(UGameplayStatics::GetActorOfClass(GetWorld(), ABulletPool::StaticClass()));
	
}
#pragma optimize("", on)

// Called every frame
void AExplosivePotion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AExplosivePotion::Explode()
{
	// Find all actors within the range sphere
	TArray<AActor*> OverlappingActors;
	AOESphere->GetOverlappingActors(OverlappingActors);

	if (OverlappingActors.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No actors found in range"));
		return;
	}

	for (int i = 0; i < OverlappingActors.Num(); i++)
	{
		if(AEnemyCharacter* enemy = Cast<AEnemyCharacter>(OverlappingActors[i]))
		{
			enemy->SetCurrentHealth(damage);
		}
	}

	pool->PutObjectBack("ExplosivePotion", this);
	SetActorLocation(FVector(0,0,0));
	UnregisterFromCollision();
}

void AExplosivePotion::DetectEnemy(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if(OtherActor && OtherActor->ActorHasTag(FName("Enemy")))
	{
		Explode();
	}
}

void AExplosivePotion::RegisterToCollision() const
{
	if(ScriptDelegate1.IsBound())
	{
		ExplosivePotionMesh->OnComponentHit.AddUnique(ScriptDelegate1);
		ExplosivePotionMesh->BodyInstance.SetInstanceSimulatePhysics(false);
		//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, BulletMesh->OnComponentHit.Contains(ScriptDelegate) ? "true" : "false");
	}
}

void AExplosivePotion::UnregisterFromCollision() const
{
	if(ScriptDelegate1.IsBound())
	{
		ExplosivePotionMesh->BodyInstance.SetInstanceSimulatePhysics(false);
		ExplosivePotionMesh->OnComponentHit.Remove(ScriptDelegate1);

		//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, BulletMesh->OnComponentHit.Contains(ScriptDelegate) ? "true" : "false");
	}
}

void AExplosivePotion::OnSpawnedFromPool(AActor* Requestee)
{
	RegisterToCollision();
}

