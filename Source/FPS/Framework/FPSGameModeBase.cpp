// Copyright Epic Games, Inc. All Rights Reserved.


#include "FPSGameModeBase.h"


#include "Engine/DemoNetDriver.h"
#include "FPS/Player/PlayerCharacter.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"


AFPSGameModeBase::AFPSGameModeBase()
{
	SpawnDelay = 3.f;
}

void AFPSGameModeBase::Respawn_Implementation(AController* Controller)
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

void AFPSGameModeBase::SwapPlayerControllers(APlayerController* OldPC, APlayerController* NewPC)
{
	Super::SwapPlayerControllers(OldPC, NewPC);
	
	AllPlayerControllers.Add(NewPC);
}

void AFPSGameModeBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSGameModeBase, AllPlayerControllers)
}

AActor* AFPSGameModeBase::GetSpawnPoint() const
{
	const int32 NumberSpawnPoint = FMath::RandRange(0, PlayerStartPoints.Num() - 1);

	return PlayerStartPoints[NumberSpawnPoint];
}

void AFPSGameModeBase::Spawn_Implementation(AController* Controller)
{
	if(AActor* StartPoint = GetSpawnPoint())
	{
		const FVector SpawnLocation = StartPoint->GetActorLocation();
		const FRotator SpawnRotation = StartPoint->GetActorRotation();
				
		if(APawn* Pawn = GetWorld()->SpawnActor<APawn>(DefaultPawnClass, SpawnLocation, SpawnRotation))
		{
			Controller->Possess(Pawn);
			Cast<APlayerCharacter>(Pawn)->UpdateHealthBar();
		}
	}
}
