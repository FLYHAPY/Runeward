// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplosivePotion.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Actor.h"
#include "Runeward/EnemyBase.h"

// Sets default values
AExplosivePotion::AExplosivePotion()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ExplosivePotionMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ExplosiveMesh"));
	RootComponent = ExplosivePotionMesh;

	AOESphere = CreateDefaultSubobject<USphereComponent>(TEXT("AOESphere"));
	AOESphere->SetupAttachment(RootComponent);

	int range = 1000.0f;
	AOESphere->SetSphereRadius(range);

	
	ExplosivePotionMesh->SetSimulatePhysics(true);
}

// Called when the game starts or when spawned
#pragma optimize("", off)
void AExplosivePotion::BeginPlay()
{
	Super::BeginPlay();

	FScriptDelegate ScriptDelegate1;
	ScriptDelegate1.BindUFunction(this, "DetectEnemy");
	// ExplosivePotionMesh->OnComponentHit.Add(ScriptDelegate1);


	if(ScriptDelegate1.IsBound())
	{
		if(ExplosivePotionMesh)
		{
			ExplosivePotionMesh->OnComponentHit.AddUnique(ScriptDelegate1);
		}else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, ExplosivePotionMesh->OnComponentHit.Contains(ScriptDelegate1) ? "true" : "false");
		}
	}
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
		if(AEnemyBase* enemy = Cast<AEnemyBase>(OverlappingActors[i]))
		{
			UE_LOG(LogTemp, Warning, TEXT("Dealt damage to: %s"), *OverlappingActors[i]->GetName());
		}
	}
	Destroy();
}

void AExplosivePotion::DetectEnemy(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if(OtherActor && OtherActor->ActorHasTag(FName("Enemy")))
	{
		Explode();
	}
}

