// Copyright Epic Games, Inc. All Rights Reserved.

#include "RunewardCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h"
#include "Towers/BulletPool.h"
#include "Towers/PoolSpawnable.h"
#include "DrawDebugHelpers.h"  // Required for debug drawing functions

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ARunewardCharacter

ARunewardCharacter::ARunewardCharacter()
{
	Tags.Add(FName("Player"));
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	health = 100;
}

void ARunewardCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	
	TArray<AActor*> FoundPool;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Pool"), FoundPool);

	if(FoundPool.Num() >= 1)
	{
		pool = Cast<ABulletPool>(FoundPool[0]);
	}
}

void ARunewardCharacter::SpawnCannon()
{
	if(!pool)
	{
		return;
	}

	// Variables
	float TraceRadius = 100.0f;
	FVector PlayerLocation = GetActorLocation();
	FVector ForwardVector = GetActorForwardVector();
	float SpawnDistance = 200.0f;

	// Calculate spawn location in front of player
	FVector SpawnLocation = PlayerLocation + (ForwardVector * SpawnDistance) + FVector(0, 0, -100);

	FCollisionQueryParams TraceParams(FName(TEXT("SpawnTrace")), true, this);
	TraceParams.bTraceComplex = true;
	TraceParams.bReturnPhysicalMaterial = false;

	// Perform the multi-sphere trace
	TArray<FHitResult> HitResults;
	bool bHit = GetWorld()->SweepMultiByChannel(
		HitResults,
		PlayerLocation,
		SpawnLocation + FVector(0, 0, 50),
		FQuat::Identity,
		ECC_Visibility,  // Choose the appropriate channel for your obstacles
		FCollisionShape::MakeSphere(TraceRadius),
		TraceParams
	);

	// Draw debug sphere at the spawn location
	DrawDebugSphere(
		GetWorld(),
		SpawnLocation + FVector(0, 0, 50),
		TraceRadius,
		12,             // Sphere segments (higher value = smoother sphere)
		FColor::Green,  // Color of the end sphere
		false,          // Persistent (false means it will disappear after duration)
		5.0f            // Duration (seconds)
	);

	bool bSpawnAllowed = true;
	for (const FHitResult& Hit : HitResults)
	{
		// Get the hit component (e.g., the component that was hit by the trace)
		UPrimitiveComponent* HitComponent = Hit.GetComponent();
		if (Hit.GetActor())
		{
			// Log only when an obstruction is found, ignoring the floor
			if (Hit.GetActor()->ActorHasTag("Floor"))
			{
				continue;
			}
        
			// Check if this component has the desired tag
			else if (HitComponent && HitComponent->ComponentHasTag("AOE"))
			{
				// If the component has the "IgnoreInSpawnTrace" tag, we can skip this hit
				UE_LOG(LogTemp, Warning, TEXT("Found an ignored component: %s"), *HitComponent->GetName());
				continue;  // Skip this hit since we don't want to block spawn
			}
			else
			{
				bSpawnAllowed = false;
				UE_LOG(LogTemp, Warning, TEXT("Obstruction detected! Cannot spawn cube. Actor: %s"), *Hit.GetActor()->GetName());
				break;  // Stop checking further, as we found an obstruction
			}
		}
	}

	
	// Check if there's an obstruction
	if (bSpawnAllowed)
	{
		// No obstruction or floor hit, spawn the cube
		AActor* tower = pool->TakeObjectOut(TowerToSpawn[index]);

		if(!tower)
		{
			return;
		}
		
		tower->SetActorLocation(SpawnLocation, false, nullptr, ETeleportType::ResetPhysics);

		if(IPoolSpawnable* Spawnable = Cast<IPoolSpawnable>(tower))
		{
			Spawnable->OnSpawnedFromPool(this);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Obstruction detected! Cannot spawn cube."));
	}
	
	/*
	UWorld* World = GetWorld();
	FRotator SpawnRotation = FRotator::ZeroRotator;
	FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	World->SpawnActor<ATowerBaseClass>(Cannon, GetActorLocation(), SpawnRotation, SpawnParameters);
	*/
}


//////////////////////////////////////////////////////////////////////////
// Input

void ARunewardCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARunewardCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ARunewardCharacter::Look);

		//Spawn Cannon
		EnhancedInputComponent->BindAction(SpawnCannonAction, ETriggerEvent::Started, this, &ARunewardCharacter::SpawnCannon);

		EnhancedInputComponent->BindAction(KeyQPressed, ETriggerEvent::Started, this, &ARunewardCharacter::OnQKeyPressed);

		EnhancedInputComponent->BindAction(KeyEPressed, ETriggerEvent::Started, this, &ARunewardCharacter::OnEKeyPressed);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ARunewardCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ARunewardCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ARunewardCharacter::OnQKeyPressed()
{
	index-= 1;
	if(index <= -1)
	{
		index = 1;
	}
	UE_LOG(LogTemp, Warning, TEXT("Player's health: %d"), index);

}

void ARunewardCharacter::OnEKeyPressed()
{
	index+=1;

	if(index >= TowerToSpawn.Num())
	{
		index = 0;
	}
	UE_LOG(LogTemp, Warning, TEXT("Player's health: %d"), index);
}

FName ARunewardCharacter::ReturnIndex()
{
	return TowerToSpawn[index];
}

void ARunewardCharacter::TakeDamage(float damage)
{
	health -= damage;
}
