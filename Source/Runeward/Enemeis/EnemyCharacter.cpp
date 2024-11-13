#include "EnemyCharacter.h"
#include "EnemyAIController.h"
#include "ARuneTower.h"
#include "Runeward/RunewardCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "TimerManager.h"

// Constructor
AEnemyCharacter::AEnemyCharacter()
{
    AIControllerClass = AEnemyAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    PrimaryActorTick.bCanEverTick = true;

    // Finds and sets up the "Sword" component as the attack area
    SwordCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SwordCollisionBox"));
    SwordCollisionBox->SetupAttachment(RootComponent);

    // Sets up the collision profile
    SwordCollisionBox->SetCollisionProfileName(TEXT("OverlapAllDynamic")); 
    SwordCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemyCharacter::OnSwordOverlapBegin);
    SwordCollisionBox->OnComponentEndOverlap.AddDynamic(this, &AEnemyCharacter::OnSwordOverlapEnd);

    // Cooldown control for attack
    bCanAttack = true;
    bIsPlayerInAttackRange = false;
    AttackCooldown = 1.0f;
}

// Function called on start
void AEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();
}

// Function called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // If the player is in the attack area and the cooldown has finished, apply damage
    if (bIsPlayerInAttackRange && bCanAttack)
    {
        ApplyDamageToPlayer();
    }
}

void AEnemyCharacter::OnSwordOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // Check if the other actor is the player
    ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (PlayerCharacter && OtherActor == PlayerCharacter)
    {
        bIsPlayerInAttackRange = true;
        ApplyDamageToPlayer();  // Applies damage to the player
    }

    // Check if the other actor is a tower
    AARuneTower* RuneTower = Cast<AARuneTower>(OtherActor);
    if (RuneTower)
    {
        ApplyDamageToTower(RuneTower); 
    }
}

// Function called when exiting overlap with the player
void AEnemyCharacter::OnSwordOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (PlayerCharacter && OtherActor == PlayerCharacter)
    {
        bIsPlayerInAttackRange = false;
    }
}

// Function that applies damage to the player and starts the cooldown
void AEnemyCharacter::ApplyDamageToPlayer()
{
    ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (PlayerCharacter)
    {
        ARunewardCharacter* RunewardCharacter = Cast<ARunewardCharacter>(PlayerCharacter);
        if (RunewardCharacter)
        {
            RunewardCharacter->TakeDamage(10.0f); 
            bCanAttack = false;
            GetWorldTimerManager().SetTimer(AttackCooldownTimer, this, &AEnemyCharacter::ResetAttack, AttackCooldown, false);
        }
    }
}

// Function to apply damage to the tower
void AEnemyCharacter::ApplyDamageToTower(AARuneTower* Tower)
{
    if (Tower && bCanAttack)
    {
        Tower->TakeDamage(10.0f);
        bCanAttack = false;
        GetWorldTimerManager().SetTimer(AttackCooldownTimer, this, &AEnemyCharacter::ResetAttack, AttackCooldown, false);
    }
}

// Function that resets the attack cooldown
void AEnemyCharacter::ResetAttack()
{
    bCanAttack = true;
}
