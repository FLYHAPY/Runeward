// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PoolSpawnable.h"
#include "TowerCost.h"
#include "GameFramework/Actor.h"
#include "ExplosivePotion.generated.h"

UCLASS()
class RUNEWARD_API AExplosivePotion : public AActor, public IPoolSpawnable, public ITowerCost
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExplosivePotion();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category="Components")
	class UStaticMeshComponent* ExplosivePotionMesh;

	// Sphere collision to detect Enemies to damage
	UPROPERTY(VisibleAnywhere, Category="Components")
	class USphereComponent* AOESphere;

	// Sphere collision to detect Enemies to damage
	UPROPERTY(VisibleAnywhere, Category="Components")
	class USphereComponent* CollidedEnemySphere;

	UPROPERTY(EditAnywhere, Category="Stats")
	float damage;

	FScriptDelegate ScriptDelegate1;

	UPROPERTY(EditAnywhere, Category="Refrecens")
	class ABulletPool* pool;

	UPROPERTY(EditAnywhere, Category="Stats")
	float cost;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Explode();

	UFUNCTION()
	void DetectEnemy(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void RegisterToCollision() const;
	void UnregisterFromCollision() const;

	virtual float TowerCost() override;

	virtual void OnSpawnedFromPool(AActor* Requestee) override;

};
