#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "ARuneTower.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemyCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// Collision overlap function for attack
	UFUNCTION()
	void OnSwordOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Collision end overlap function
	UFUNCTION()
	void OnSwordOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// Function to reset attack ability after cooldown
	void ResetAttack();

	// Function to apply damage to the player and start cooldown
	void ApplyDamageToPlayer();

	// Function to apply damage to the tower
	void ApplyDamageToTower(AARuneTower* Tower);

private:
	// Collision box for the enemy's "sword"
	UPROPERTY(VisibleAnywhere, Category = "Attack")
	UBoxComponent* SwordCollisionBox;

	// Attack and cooldown control
	bool bCanAttack;
	bool bIsPlayerInAttackRange;
	FTimerHandle AttackCooldownTimer;
	float AttackCooldown;
};
