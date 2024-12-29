#pragma once


#include "GameFramework/Actor.h"
#include "ARuneTower.generated.h"

UCLASS()
class RUNEWARD_API AARuneTower : public AActor
{
	GENERATED_BODY()
	
public:	
	AARuneTower();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, Category = "Tower")
	float Health = 100.0f;
};
