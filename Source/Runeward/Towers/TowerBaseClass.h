// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
using namespace std;
#include "TowerBaseClass.generated.h"


UCLASS()
class RUNEWARD_API ATowerBaseClass : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATowerBaseClass();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Tower's fire rate
	UPROPERTY(EditAnywhere, Category="Tower")
	float fireRate;

	// Range of the tower (radius of the sphere)
	UPROPERTY(EditAnywhere, Category="Tower")
	float range;

	// Pointer to the currently locked enemy
	UPROPERTY(EditAnywhere, Category="Tower")
	AActor* lockedEnemy;
	
	// Sphere collision to detect enemies
	UPROPERTY(VisibleAnywhere, Category="Components")
	class USphereComponent* RangeSphere;

	// Tower mesh
	UPROPERTY(VisibleAnywhere, Category="Components")
	class UStaticMeshComponent* TowerMesh;

	// Whether the tower has locked onto an enemy
	bool hasLockedEnemy;

	float timer;

	UPROPERTY(EditAnywhere, Category="Refrecens")
	class ABulletPool* pool;



public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	void CheckForEnemies();

	UFUNCTION()
	AActor* GetLockedEnemy();
	
	UFUNCTION()
	void Shoot();

	UFUNCTION()
	void OnEnemyExitRange(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnEnemyEnterRange(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
