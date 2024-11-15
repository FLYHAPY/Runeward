#include "EnemyCharacter.h"
#include "EnemyAIController.h"
#include "Runeward/Towers/MainTower.h"
#include "Runeward/RunewardCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "Runeward/Towers/Bullet.h"
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

    // Sets up the collision profile
    SwordCollisionBox->SetCollisionProfileName(TEXT("OverlapAllDynamic")); 
    SwordCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemyCharacter::OnSwordOverlapBegin);
    SwordCollisionBox->OnComponentEndOverlap.AddDynamic(this, &AEnemyCharacter::OnSwordOverlapEnd);

    // Cooldown control for attack
    bCanAttack = true;
    bIsPlayerInAttackRange = false;
    AttackCooldown = 1.0f;

    Tags.Add(FName("Enemy"));

    Health = 10;
    maxHeath = 10;
}

// Function called on start
void AEnemyCharacter::BeginPlay()
{
    
    Super::BeginPlay();

    ScriptDelegate.BindUFunction(this, "OnBulletHit");

    UCapsuleComponent* capsule = this->FindComponentByClass<UCapsuleComponent>();

    if(ScriptDelegate.IsBound())
    {
      capsule->OnComponentHit.Add(ScriptDelegate);
    }

    pool = Cast<ABulletPool>(UGameplayStatics::GetActorOfClass(GetWorld(), ABulletPool::StaticClass()));
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


    if (AMainTower* RuneTower = Cast<AMainTower>(OtherActor))
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
void AEnemyCharacter::ApplyDamageToTower(AMainTower* Tower)
{
    if (Tower && bCanAttack)
    {
        Tower->TakeDamage(10.0f);
        bCanAttack = false;
        GetWorldTimerManager().SetTimer(AttackCooldownTimer, this, &AEnemyCharacter::ResetAttack, AttackCooldown, false);
    }
}

void AEnemyCharacter::OnBulletHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,FVector NormalImpulse, const FHitResult& Hit)
{
    
    if(OtherActor && OtherActor->ActorHasTag(FName("Bullet")))
    {
        if(ABullet* myBullet = Cast<ABullet>(OtherActor))
        {
            SetCurrentHealth(myBullet->GetDamage());
        }
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

// Function that resets the attack cooldown
void AEnemyCharacter::ResetAttack()
{
    bCanAttack = true;
}
