// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplosivePotion.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AExplosivePotion::AExplosivePotion()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ExplosivePotionMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ExplosiveMesh"));
	RootComponent = ExplosivePotionMesh;

	AOESphere = CreateDefaultSubobject<USphereComponent>(TEXT("AOESphere"));
	AOESphere->SetupAttachment(RootComponent);

	CollidedEnemySphere = CreateDefaultSubobject<USphereComponent>(TEXT("RangeSphere"));
	CollidedEnemySphere->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AExplosivePotion::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AExplosivePotion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

