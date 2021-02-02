// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInfoInstance.h"

#include "Engine/DemoNetDriver.h"


void UGameInfoInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UGameInfoInstance, MaxPlayers);
	DOREPLIFETIME(UGameInfoInstance, ServerName);
}

