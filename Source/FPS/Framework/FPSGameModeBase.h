// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FPSGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class FPS_API AFPSGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AFPSGameModeBase();

	void Respawn(AController* Controller);

protected:
	virtual void BeginPlay() override;

private:
	AActor* GetSpawnPoint() const;
	
	UFUNCTION()
	void Spawn(AController* Controller);

public:
	UPROPERTY(EditDefaultsOnly)
	float SpawnDelay;
	
private:
	UPROPERTY()
	TArray<class AActor*> PlayerStartPoints;
};
