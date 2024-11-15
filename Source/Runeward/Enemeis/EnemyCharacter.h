#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "Runeward/Towers/MainTower.h"
#include "Runeward/Towers/PoolSpawnable.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class AEnemyCharacter : public ACharacter, public IPoolSpawnable
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
	void ApplyDamageToTower(AMainTower* Tower);
	
	UFUNCTION()
	void OnBulletHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void SetCurrentHealth(float damage);

	virtual void OnSpawnedFromPool(AActor* Requestee) override;

	void RegisterToCollision() const;
	void UnregisterFromCollision() const;

private:
	// Collision box for the enemy's "sword"
	UPROPERTY(VisibleAnywhere, Category = "Attack")
	UBoxComponent* SwordCollisionBox;

	// Attack and cooldown control
	bool bCanAttack;
	bool bIsPlayerInAttackRange;
	FTimerHandle AttackCooldownTimer;
	float AttackCooldown;

	UPROPERTY(VisibleAnywhere, Category="Stats")
	float Health;

	UPROPERTY(VisibleAnywhere, Category="Stats")
	float maxHeath;

	FScriptDelegate ScriptDelegate;

	UPROPERTY(EditAnywhere, Category="Refrecens")
	class ABulletPool* pool;
};
