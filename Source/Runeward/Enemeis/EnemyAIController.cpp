#include "EnemyAIController.h"  
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Runeward/RunewardCharacter.h"
#include "Runeward/Towers/MainTower.h"

void AEnemyAIController::BeginPlay()
{
    Super::BeginPlay();

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
    ACharacter* ClosestPlayer = GetClosestPlayer();

    if (ClosestPlayer && GetPawn())
    {
        MoveToActor(ClosestPlayer, 5.0f);
    }
}

void AEnemyAIController::MoveToRuneTower()
{
    if (RuneTower && GetPawn())
    {
        MoveToActor(RuneTower, 5.0f);
    }
    
    // Checks every second if a player is within range
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AEnemyAIController::MoveToRuneTower, 1.0f, true);

    // If any player is within range, change the target to the closest player
    if (IsPlayerInRange())
    {
        MoveToPlayer();
    }
}

bool AEnemyAIController::IsPlayerInRange()
{
    ACharacter* ClosestPlayer = GetClosestPlayer();

    if (!ClosestPlayer) return false;

    // Calculates the distance to the closest player
    float DistanceToPlayer = FVector::Dist(ClosestPlayer->GetActorLocation(), GetPawn()->GetActorLocation());
    return DistanceToPlayer <= DetectionRadius;
}

ACharacter* AEnemyAIController::GetClosestPlayer()
{
    TArray<AActor*> PlayerActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARunewardCharacter::StaticClass(), PlayerActors);

    ACharacter* ClosestPlayer = nullptr;
    float MinDistance = FLT_MAX;

    for (AActor* Actor : PlayerActors)
    {
        ACharacter* PlayerCharacter2 = Cast<ACharacter>(Actor);
        if (PlayerCharacter2)
        {
            float Distance = FVector::Dist(PlayerCharacter2->GetActorLocation(), GetPawn()->GetActorLocation());
            if (Distance < MinDistance)
            {
                MinDistance = Distance;
                ClosestPlayer = PlayerCharacter2;
            }
        }
    }

    return ClosestPlayer;
}

void AEnemyAIController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AEnemyAIController, ExampleProperty);
}

void AEnemyAIController::RunOnServerExample_Implementation()
{
}
