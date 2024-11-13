#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ARuneTower.generated.h"

UCLASS()
class RUNEWARD_API AARuneTower : public AActor
{
	GENERATED_BODY()
	
public:	
	AARuneTower();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	// Function to apply damage to the tower
	void TakeDamage(float DamageAmount);

private:
	UPROPERTY(EditAnywhere, Category = "Tower")
	float Health = 100.0f;
};
