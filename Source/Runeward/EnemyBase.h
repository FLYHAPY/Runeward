// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Towers/PoolSpawnable.h"
#include "EnemyBase.generated.h"

UCLASS()
class RUNEWARD_API AEnemyBase : public AActor, public IPoolSpawnable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, Category="Components")
	class UStaticMeshComponent* EnemyMesh;

	UPROPERTY(EditAnywhere, Category="Refrecens")
	class ABulletPool* pool;

	UPROPERTY(VisibleAnywhere, Category="Stats")
	float Health;

	UPROPERTY(VisibleAnywhere, Category="Stats")
	float maxHeath;

	FScriptDelegate ScriptDelegate;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	void OnBulletHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	float GetCurrentHealth();

	UFUNCTION()
	void SetCurrentHealth(float damage);

	virtual void OnSpawnedFromPool(AActor* Requestee) override;
	
	void RegisterToCollision() const;
	void UnregisterFromCollision() const;
	
	
};
