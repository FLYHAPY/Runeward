#pragma once


#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "Runeward/Towers/MainTower.h"
#include "Runeward/Towers/PoolSpawnable.h"
#include "EnemyCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDelegate,  AEnemyCharacter*, Enemy);

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
	void SetCurrentHealth(float damage);

	virtual void OnSpawnedFromPool(AActor* Requestee) override;

	void RegisterToCollision() const;
	void UnregisterFromCollision() const;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FDelegate Delegate;

	UFUNCTION()
	float GiveCoins();
	
	UFUNCTION(BlueprintCallable, Category = "MyBlueprintCategory")
	bool isEnemyAttacking();

private:
	// Collision box for the enemy's "sword"
	UPROPERTY(VisibleAnywhere, Category = "Attack")
	UBoxComponent* SwordCollisionBox;

	// Attack and cooldown control
	UPROPERTY(VisibleAnywhere, Category="Stats")
	bool bCanAttack;
	UPROPERTY(VisibleAnywhere, Category="Stats")
	bool bIsPlayerInAttackRange;
	UPROPERTY(VisibleAnywhere, Category="Stats")
	bool bIsRuneTowerInRange;
	UPROPERTY(VisibleAnywhere, Category="Stats")
	FTimerHandle AttackCooldownTimer;
	UPROPERTY(VisibleAnywhere, Category="Stats")
	float AttackCooldown;

	UPROPERTY(VisibleAnywhere, Category="Stats")	
	bool appliedDamage;

	AMainTower* RuneTower;

	float coins;

	// Tower mesh
	UPROPERTY(VisibleAnywhere, Category="Components")
	class UStaticMeshComponent* Sword;

	UPROPERTY(VisibleAnywhere, Category="Stats")
	float Health;

	UPROPERTY(VisibleAnywhere, Category="Stats")
	float maxHeath;

	FScriptDelegate ScriptDelegate;

	UPROPERTY(EditAnywhere, Category="Refrecens")
	class ABulletPool* pool;

	UPROPERTY(Replicated)
	bool isAttacking;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	FScriptDelegate swordScriptDelegate;

	UPROPERTY(VisibleAnywhere, Category="Components")
	class UStaticMeshComponent* swordMesh;

	UFUNCTION()
	void OnSwordHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
