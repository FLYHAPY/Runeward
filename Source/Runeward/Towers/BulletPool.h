// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bullet.h"
#include "CoreMinimal.h"
#include "TowerBaseClass.h"
#include "GameFramework/Actor.h"
#include "BulletPool.generated.h"



UCLASS()
class RUNEWARD_API ABulletPool : public AActor
{
	GENERATED_BODY()
		
public:	
	// Sets default values for this actor's properties
	ABulletPool();

	UFUNCTION()
	void TakeBulletFromQueue(ABullet* Bullet);
	
	UFUNCTION()
	void CacheBulletsIntoQueue();

	UFUNCTION()
	void PortBull(FVector position);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> Bullet;

	UPROPERTY(EditAnywhere)
	FVector SpawnLocation;

	UPROPERTY(EditAnywhere)
	int bulletsToSpawn;

	// Ensure Pointers Use A UPROPERTY() To Avoid Dangling Pointers
	TQueue <TWeakObjectPtr<AActor>> AllBullets;

	UPROPERTY()
	TArray <TWeakObjectPtr<AActor>> ActiveBullets;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
