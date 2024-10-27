// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExplosivePotion.generated.h"

UCLASS()
class RUNEWARD_API AExplosivePotion : public AActor
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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
