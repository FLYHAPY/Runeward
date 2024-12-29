#pragma once


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

	//eXAMPLE sAKE
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	UPROPERTY(Replicated)
	float ExampleProperty;

	UFUNCTION(Server, Reliable)
	void RunOnServerExample();

	UFUNCTION()
	ACharacter* GetClosestPlayer();
	
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
