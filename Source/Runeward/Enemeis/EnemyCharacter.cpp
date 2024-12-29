#include "EnemyCharacter.h"
#include "EnemyAIController.h"
#include "Runeward/RunewardCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"
#include "Runeward/Towers/BulletPool.h"

// Constructor
AEnemyCharacter::AEnemyCharacter()
{
    AIControllerClass = AEnemyAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    
    PrimaryActorTick.bCanEverTick = true;

    // Finds and sets up the "Sword" component as the attack area
    SwordCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SwordCollisionBox"));
    SwordCollisionBox->SetupAttachment(RootComponent);

    swordMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TowerMesh"));

    swordMesh->SetupAttachment(GetMesh(), FName("hand_rSocket"));

    // Sets up the collision profile
    SwordCollisionBox->SetCollisionProfileName(TEXT("OverlapAllDynamic")); 
    SwordCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemyCharacter::OnSwordOverlapBegin);
    SwordCollisionBox->OnComponentEndOverlap.AddDynamic(this, &AEnemyCharacter::OnSwordOverlapEnd);

    // Cooldown control for attack
    bCanAttack = true;
    bIsPlayerInAttackRange = false;
    AttackCooldown = 2.2f;

    Tags.Add(FName("Enemy"));

    coins = 5;


    Health = 10;
    maxHeath = 10;
    appliedDamage = false;
}

// Function called on start
void AEnemyCharacter::BeginPlay()
{
    
    Super::BeginPlay();

    swordScriptDelegate.BindUFunction(this, "OnSwordHit");

    if(swordScriptDelegate.IsBound())
    {
        swordMesh->OnComponentHit.Add(swordScriptDelegate);
    }

    pool = Cast<ABulletPool>(UGameplayStatics::GetActorOfClass(GetWorld(), ABulletPool::StaticClass()));

    RuneTower = Cast<AMainTower>(UGameplayStatics::GetActorOfClass(GetWorld(), AMainTower::StaticClass()));

    AEnemyAIController* EnemyAIController = Cast<AEnemyAIController>(this->GetController());
    if (!EnemyAIController)
    {
        EnemyAIController = GetWorld()->SpawnActor<AEnemyAIController>(AEnemyAIController::StaticClass());
        EnemyAIController->Possess(this);
    }
}

// Function called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // If the player is in the attack area and the cooldown has finished, apply damage
    if (bIsPlayerInAttackRange && bCanAttack)
    {
        isAttacking = true;
        bCanAttack = false;
        GetWorldTimerManager().SetTimer(AttackCooldownTimer, this, &AEnemyCharacter::ResetAttack, AttackCooldown, false);
        //ApplyDamageToPlayer();
    }

    if(bIsRuneTowerInRange && bCanAttack)
    {
        ApplyDamageToTower(RuneTower);  
    }
}

//this 2 functions purpose is to check if there is a player or RuneTower in range
void AEnemyCharacter::OnSwordOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    TArray<AActor*> PlayerActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARunewardCharacter::StaticClass(), PlayerActors);

    for(int32 i = 0; i < PlayerActors.Num(); i++)
    {
        if(PlayerActors[i] == OtherActor)
        {
            bIsPlayerInAttackRange = true;
        }
    }


    if (RuneTower == OtherActor)
    {
        bIsRuneTowerInRange = true;
        ApplyDamageToTower(RuneTower);  
    }
}

void AEnemyCharacter::OnSwordOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    TArray<AActor*> PlayerActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARunewardCharacter::StaticClass(), PlayerActors);

    for(int32 i = 0; i < PlayerActors.Num(); i++)
    {
        if(PlayerActors[i] == OtherActor)
        {
            bIsPlayerInAttackRange = false;
        }
    }

    if (OtherActor == RuneTower)
    {
        bIsRuneTowerInRange = false;
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
            RunewardCharacter->TakeDamage1(10.0f); 
            bCanAttack = false;
            //GetWorldTimerManager().SetTimer(AttackCooldownTimer, this, &AEnemyCharacter::ResetAttack, AttackCooldown, false);
        }
    }
}

// Function to apply damage to the tower
void AEnemyCharacter::ApplyDamageToTower(AMainTower* Tower)
{
    if (Tower && bCanAttack)
    {
        Tower->TakeDamage2(10.0f);
        bCanAttack = false;
        GetWorldTimerManager().SetTimer(AttackCooldownTimer, this, &AEnemyCharacter::ResetAttack, AttackCooldown, false);
    }
}

void AEnemyCharacter::SetCurrentHealth(float towerDamage)
{
    Health -= towerDamage;

    if(Health <= 0)
    {
        SetActorLocation(FVector(0, 0, 0));
        pool->PutObjectBack("Enemy", this);
        UnregisterFromCollision();
        Delegate.Broadcast(this);
    }
}

void AEnemyCharacter::OnSpawnedFromPool(AActor* Requestee)
{
    RegisterToCollision();
    Health = maxHeath;
}

void AEnemyCharacter::RegisterToCollision() const
{
    if(ScriptDelegate.IsBound())
    {
        UCapsuleComponent* capsule = this->FindComponentByClass<UCapsuleComponent>();
        capsule->OnComponentHit.AddUnique(ScriptDelegate);
        //GetCapsuleComponent()->BodyInstance.SetInstanceSimulatePhysics(true);
        //GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, BulletMesh->OnComponentHit.Contains(ScriptDelegate) ? "true" : "false");
    }
}

void AEnemyCharacter::UnregisterFromCollision() const
{
    
    if(ScriptDelegate.IsBound())
    {
        UCapsuleComponent* capsule = this->FindComponentByClass<UCapsuleComponent>();
        capsule->OnComponentHit.Remove(ScriptDelegate);
        //GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, BulletMesh->OnComponentHit.Contains(ScriptDelegate) ? "true" : "false");
    }
}

bool AEnemyCharacter::isEnemyAttacking()
{
    return isAttacking;
}

void AEnemyCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AEnemyCharacter, isAttacking);
}

void AEnemyCharacter::OnSwordHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    //UE_LOG(LogTemp, Warning, TEXT("sword:  %s"), *OtherActor->GetName());
    if(isAttacking && OtherActor && OtherActor == PlayerCharacter && !appliedDamage)
    {
        appliedDamage = true;
        ApplyDamageToPlayer();
    }
}

float AEnemyCharacter::GiveCoins()
{
    return coins;
}

// Function that resets the attack cooldown
void AEnemyCharacter::ResetAttack()
{
    appliedDamage = false;
    isAttacking = false;
    bCanAttack = true;
    GetWorldTimerManager().ClearTimer(AttackCooldownTimer);
}
