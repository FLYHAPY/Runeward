// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MyGameStateBase.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameState.h"
#include "Logging/LogMacros.h"
#include "RunewardCharacter.generated.h"

class AGameState;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;	

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ARunewardCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Spawn Cannon Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SpawnCannonAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* KeyQPressed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* KeyEPressed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AttackPressed;

public:
	ARunewardCharacter();
	
protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, Category="Refrecens")
	class ABulletPool* pool;

	int index = 0;

	UPROPERTY(EditAnywhere, Category="stats")
	float health;

	UPROPERTY(EditAnywhere, Category="stats")
	float maxHealth;

	UPROPERTY(EditAnywhere, Category="stats")
	float attackColdown;

	UPROPERTY(VisibleAnywhere, Category="Stats")
	FTimerHandle attackCooldownTimer;

	UPROPERTY(EditAnywhere, Category="Refrecens")
	TArray <FName> TowerToSpawn;

	UPROPERTY(EditAnywhere, Category="Refrecens")
	AMyGameStateBase* GameState;

	UPROPERTY(Replicated)
	bool isAttacking;

	FScriptDelegate ScriptDelegate4;

	UPROPERTY(EditAnywhere, Category="Points")
	float necessaryCoins;

	UPROPERTY()
	UStaticMeshComponent* weaponMesh;
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> Cannon;

public:

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UFUNCTION(Blueprintable, Category = Functions)
	void SpawnCannon();

	UFUNCTION(Blueprintable, Category = Functions)
	void OnQKeyPressed();

	UFUNCTION(Blueprintable, Category = Functions)
	void OnEKeyPressed();

	UFUNCTION(BlueprintPure, Category = Functions)
	FName ReturnIndex();

	UFUNCTION(BlueprintCallable, Category = Functions)
	void TakeDamage1(float damage);

	UFUNCTION(BlueprintPure, Category = Functions)
	float ReturnNormalizedHealth();

	UFUNCTION(BlueprintPure, Category = Functions)
	bool ReturnIsAttacking();

	UFUNCTION(BlueprintCallable, Category = Functions)
	void OnAttack();

	UFUNCTION(BlueprintCallable, Category = Functions)
	void ResetAttack();

	UFUNCTION()
	void OnPlayerSwordHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(Server, Reliable)
	void Server_DealDamageTOEnemy(AEnemyCharacter* enemy);

	UFUNCTION(Server, Reliable)
	void Server_SpawnTowerOnServer(FVector SpawnLocation, FName TowerToSpawnFname);
};

