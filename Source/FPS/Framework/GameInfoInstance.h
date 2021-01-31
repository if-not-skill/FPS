// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameInfoInstance.generated.h"

/**
 * 
 */
UCLASS()
class FPS_API UGameInfoInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(Replicated, BlueprintReadWrite)
	int32 MaxPlayers;
	
	UPROPERTY(Replicated, BlueprintReadWrite)
	FText ServerName;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
};
