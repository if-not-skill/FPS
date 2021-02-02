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

	UFUNCTION(Server, Reliable)
	void Respawn(AController* Controller);

protected:
	virtual void BeginPlay() override;
	virtual void SwapPlayerControllers(APlayerController* OldPC, APlayerController* NewPC) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	AActor* GetSpawnPoint() const;
	
	UFUNCTION(Server, Reliable)
	void Spawn(AController* Controller);

public:
	UPROPERTY(EditDefaultsOnly)
	float SpawnDelay;

	UPROPERTY(Replicated, BlueprintReadWrite)
	TArray<APlayerController*> AllPlayerControllers;
	
private:
	UPROPERTY()
	TArray<class AActor*> PlayerStartPoints;
};
