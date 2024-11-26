// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MainTower.generated.h"

UCLASS()
class RUNEWARD_API AMainTower : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMainTower();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category="Components")
	class UStaticMeshComponent* MainToweMesh;
	

	UPROPERTY(EditAnywhere, Category = "Tower")
	float Health = 0.0f;
	float maxHealth = 100.0f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Function to apply damage to the tower
	void TakeDamage(float DamageAmount);

	UFUNCTION(BlueprintPure)
	float ReturnHealth();
	

};
