// Fill out your copyright notice in the Description page of Project Settings.


#include "MainTower.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
AMainTower::AMainTower()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MainToweMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TowerMesh"));
	RootComponent = MainToweMesh;

	maxHealth = 100;
	Health = maxHealth;

}

// Called when the game starts or when spawned
void AMainTower::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMainTower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Function to apply damage to the tower
void AMainTower::TakeDamage2(float DamageAmount)
{
	Health -= DamageAmount;
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Torre recebeu dano: %f, Vida restante: %f"), DamageAmount, Health));
	}
	if (Health <= 0.0f)
	{
		FName NextLevelName = "GameOver";
		UGameplayStatics::OpenLevel(GetWorld(), NextLevelName);
	}
}

float AMainTower::ReturnHealth()
{
	return Health /maxHealth;
}

