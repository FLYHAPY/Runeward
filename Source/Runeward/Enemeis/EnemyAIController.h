#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

UCLASS()
class AEnemyAIController : public AAIController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	// Functions to move toward the player and the tower
	void MoveToPlayer();
	void MoveToRuneTower();

	// Function to check distance to the player
	bool IsPlayerInRange();

private:
	FTimerHandle TimerHandle;

	// References to the player and tower
	ACharacter* PlayerCharacter;
	AActor* RuneTower;

	// Detection distance for the player
	UPROPERTY(EditAnywhere, Category="AI")
	float DetectionRadius = 1200.0f;
	virtual void OnPossess(APawn* InPawn) override;
};
