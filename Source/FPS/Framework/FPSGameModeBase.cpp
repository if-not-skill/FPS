// Copyright Epic Games, Inc. All Rights Reserved.


#include "FPSGameModeBase.h"


#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

AFPSGameModeBase::AFPSGameModeBase()
{
	SpawnDelay = 3.f;
}

void AFPSGameModeBase::Respawn(AController* Controller)
{
	if(Controller){
		if(GetLocalRole() == ROLE_Authority)
		{
			FTimerDelegate TimerDel_Spawn;
			FTimerHandle TimerHandle_Spawn;

			TimerDel_Spawn.BindUFunction(this, FName("Spawn"), Controller);
			GetWorldTimerManager().SetTimer(TimerHandle_Spawn, TimerDel_Spawn, SpawnDelay, false);
		}
	}
}

void AFPSGameModeBase::BeginPlay()
{
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStartPoints);
}

AActor* AFPSGameModeBase::GetSpawnPoint() const
{
	const int32 NumberSpawnPoint = FMath::RandRange(0, PlayerStartPoints.Num() - 1);

	return PlayerStartPoints[NumberSpawnPoint];
}

void AFPSGameModeBase::Spawn(AController* Controller)
{
	if(AActor* StartPoint = GetSpawnPoint())
	{
		const FVector SpawnLocation = StartPoint->GetActorLocation();
		const FRotator SpawnRotation = StartPoint->GetActorRotation();
				
		if(APawn* Pawn = GetWorld()->SpawnActor<APawn>(DefaultPawnClass, SpawnLocation, SpawnRotation))
		{
			Controller->Possess(Pawn);
		}
	}
}
