#include "ARuneTower.h"

// Sets default values
AARuneTower::AARuneTower()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AARuneTower::BeginPlay()
{
	Super::BeginPlay();
}

void AARuneTower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Function to apply damage to the tower
void AARuneTower::TakeDamage(float DamageAmount)
{
	Health -= DamageAmount;
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Torre recebeu dano: %f, Vida restante: %f"), DamageAmount, Health));
	}
	if (Health <= 0.0f)
	{
		Destroy();
	}
}

