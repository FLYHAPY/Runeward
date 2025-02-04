// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/PlayerStart.h"
#include "EnumLib.h"
#include "StructLib.h"
#include "IngameDeathmatchGamemode.generated.h"

/**
 * 
 */
UCLASS()
class RUNEWARD_API AIngameDeathmatchGamemode : public AGameMode
{
	GENERATED_BODY()

public: // Gameplay Functions

	AIngameDeathmatchGamemode();
	
	virtual void BeginPlay() override;
	virtual void StartPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Player")
	UClass* PlayerCharacter;

private: // Player Login And Character Creation
	
	/**
	 * Accept or reject a player attempting to join the server.  Fails login if you set the ErrorMessage to a non-empty string.
	 * PreLogin is called before Login.  Significant game time may pass before Login is called
	 *
	 * @param	Options					The URL options (e.g. name/spectator) the player has passed
	 * @param	Address					The network address of the player
	 * @param	UniqueId				The unique id the player has passed to the server
	 * @param	ErrorMessage			When set to a non-empty value, the player will be rejected using the error message set
	 */
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	/**
	 * Called to login new players by creating a player controller, overridable by the game
	 *
	 * Sets up basic properties of the player (name, unique id, registers with backend, etc) and should not be used to do
	 * more complicated game logic.  The player controller is not fully initialized within this function as far as networking is concerned.
	 * Save "game logic" for PostLogin which is called shortly afterward.
	 *
	 * @param NewPlayer pointer to the UPlayer object that represents this player (either local or remote)
	 * @param RemoteRole the remote role this controller has
	 * @param Portal desired portal location specified by the client
	 * @param Options game options passed in by the client at login
	 * @param UniqueId platform specific unique identifier for the logging in player
	 * @param ErrorMessage [out] error message, if any, why this login will be failing
	 *
	 * If login is successful, returns a new PlayerController to associate with this player. Login fails if ErrorMessage string is set.
	 *
	 * @return a new player controller for the logged in player, NULL if login failed for any reason
	 */
	virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage);

	/** Called after a successful login.  This is the first place it is safe to call replicated functions on the
	 * PlayerController. */
	virtual void PostLogin(APlayerController* NewPlayer);

private: // Interal Helpers

	bool IsClientAuthTokenValid(const FString& InAuthToken) const;

	void DumpClientAuthTokens() const;

	ECharacterType ParseCharacterOptionToEnum(const FString& CharacterOption);

	
	// Not used right now but we can keep track of this information for letting any other system of results etc
	//TMap<APlayerController*, FPlayerConnectionData> PlayerConnectionData;
	
	UPROPERTY()
	TArray<AActor*> BlueTeamSpawns;
	
	UPROPERTY()
	TArray<AActor*> RedTeamSpawns;

protected:
	
	// Default One, But We Can Change It To Another If It Makes Sense To Do So
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FSoftObjectPath DataTablePath = FSoftObjectPath(TEXT("/Game/DT_SelectablePlayerCharacters.DT_SelectablePlayerCharacters"));

private:
	// Actual Runtime DataTable
	UPROPERTY()
	UDataTable* SelectablePlayerCharacterDataTable;
};
