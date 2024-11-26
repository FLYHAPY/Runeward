// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PoolSpawnable.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBulletHit, ABullet*, Bullet);

UCLASS()
class RUNEWARD_API ABullet : public AActor, public IPoolSpawnable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category="Bullet")
	float range;

	UPROPERTY(EditAnywhere, Category="Bullet")
	float damage;

	UPROPERTY(VisibleAnywhere, Category="Bullet")
	class USphereComponent* Collision;

	// Tower mesh
	UPROPERTY(VisibleAnywhere, Category="Components")
	class UStaticMeshComponent* BulletMesh;

	UPROPERTY(EditAnywhere, Category="Bullet")
	FVector StartLocation;

	UPROPERTY(EditAnywhere, Category="Bullet")
	float bulletSpeed;

	UPROPERTY(EditAnywhere, Category="Bullet")
	bool bulletActive;

	UPROPERTY(EditAnywhere, Category="Bullet")
	FVector target;

	UPROPERTY(EditAnywhere, Category="Refrecens")
	class ABulletPool* pool;

	UPROPERTY(EditAnywhere, Category="Bullet")
	int bounces;

	
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnBulletHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditAnywhere, Category = "Events")
	FOnBulletHit OnBulletHitDelegate;

	virtual void OnSpawnedFromPool(AActor* Requestee) override;

	void RegisterToCollision() const;
	void UnregisterFromCollision() const;
	
	FScriptDelegate ScriptDelegate;

	void SetDamage(float TowerDamage);

	float GetDamage();

	UFUNCTION()
	float GetBulletSpeed();

};
