// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"

#include "Engine/DemoNetDriver.h"
#include "FPS/Player/PlayerCharacter.h"

UHealthComponent::UHealthComponent()
{
	MaxHealthPoints = 200.f;
	CurrentHealthPoints = MaxHealthPoints;
}

float UHealthComponent::GetCurrentHealthPercentage() const
{
	return CurrentHealthPoints / MaxHealthPoints;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	if(AActor* Owner = GetOwner())
	{
		Owner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::TakeDamage);
	}
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealthComponent, CurrentHealthPoints);
}

void UHealthComponent::TakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
                                  AController* InstigatedBy, AActor* DamageCauser)
{
	if(Damage <= 0.f)
	{
		return;
	}

	CurrentHealthPoints = FMath::Clamp(CurrentHealthPoints - Damage, 0.f, MaxHealthPoints);

	if(APlayerCharacter* Char = Cast<APlayerCharacter>(DamagedActor))
	{
		Char->ShowHitVisual(DamageCauser->GetActorLocation());
		Char->HandleCameraShakeHit();
		Char->UpdateHealthBar();
		
		if(APlayerCharacter* Enemy = Cast<APlayerCharacter>(DamageCauser))
		{
			
		}
		
		if(CurrentHealthPoints == 0.f)
		{
			Char->Die();
		}
	}
}


