#include "EnemyAIController.h"
#include "NavigationSystem.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "ARuneTower.h" // Include the tower class
#include "Runeward/Towers/MainTower.h"

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	// Reference to the player
	PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	// Searches for the RuneTower in the level
	RuneTower = UGameplayStatics::GetActorOfClass(GetWorld(), AMainTower::StaticClass());
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// Start moving to the initial target (RuneTower or Player) only after possession
	MoveToRuneTower();
}


void AEnemyAIController::MoveToPlayer()
{
	if (PlayerCharacter && GetPawn())
	{
		MoveToActor(PlayerCharacter, 5.0f);
	}
}

void AEnemyAIController::MoveToRuneTower()
{
	if (RuneTower && GetPawn())
	{
		MoveToActor(RuneTower, 5.0f);
	}
	
	// Checks every second if the player is within range
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AEnemyAIController::MoveToRuneTower, 1.0f, true);

	// If the player is within range, changes the target to the player
	if (IsPlayerInRange())
	{
		MoveToPlayer();
	}
}

bool AEnemyAIController::IsPlayerInRange()
{
	if (!PlayerCharacter) return false;

	// Calculates the distance to the player
	float DistanceToPlayer = FVector::Dist(PlayerCharacter->GetActorLocation(), GetPawn()->GetActorLocation());
	return DistanceToPlayer <= DetectionRadius;
}
