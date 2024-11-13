// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletPool.generated.h"

USTRUCT(BlueprintType)
struct FConstructionList
{
	GENERATED_BODY();

	UPROPERTY(EditAnywhere)
	FName Key;

	UPROPERTY(EditAnywhere)
	int howManyObjectsToSpawn;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> ClassToSpawn;
};

UCLASS()
class RUNEWARD_API ABulletPool : public AActor
{
	GENERATED_BODY()
		
public:	
	// Sets default values for this actor's properties
	ABulletPool();

	/*
	UFUNCTION()
	void TakeBulletFromQueue(ABullet* Bullet);
	*/

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere)
	FVector SpawnLocation;

	// Ensure Pointers Use A UPROPERTY() To Avoid Dangling Pointers
	TQueue <TWeakObjectPtr<AActor>> AllBullets;

	UPROPERTY()
	TArray <TWeakObjectPtr<AActor>> ActiveBullets;
	
	TMap<FName,TArray<TWeakObjectPtr<AActor>>> MultiMap;
	
	UPROPERTY(EditAnywhere, Category="Refrecens")
	TArray <FConstructionList> ObjectsToSpawn;
	
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	AActor* TakeObjectOut(FName objectToTakeOut);

	UFUNCTION()
	void PutObjectBack(FName objectToTakeOut, TWeakObjectPtr<AActor> objectPulledOut);

private:

	UFUNCTION()
	void CreatePooledObjects();
	

	/*UFUNCTION()
	void CacheBulletsIntoQueue();

	UFUNCTION()
	void PortBull(AActor* position);*/
	
};
