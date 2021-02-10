// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeapon.h"

#include "FPS/Weapons/ProjectileMaster.h"
#include "Camera/CameraComponent.h"
#include "Engine/DemoNetDriver.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

ABaseWeapon::ABaseWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	WeaponState = EWeaponState::WS_Ready;
	
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	RootComponent = WeaponMesh;

}

float ABaseWeapon::GetCalculatedDamage(EBodyPart BodyPart, float Distance)
{	
	const float BodyMultiplier = WeaponData.DamageMultipliers[BodyPart];

	float Damage =  (WeaponData.Damage * BodyMultiplier);

	if(WeaponData.FireRate < Distance)
	{
		Distance = Distance - WeaponData.FireRate;
		Damage = Damage - (Damage * Distance * WeaponData.DamageDropMultiplier);
	}
	
	return FMath::Clamp(Damage, WeaponData.DamageMin, 300.f);
}

void ABaseWeapon::Fire()
{
	if(CurrentAmmo != 0)
	{
		const FVector SpawnLocation = WeaponMesh->GetSocketLocation("Muzzle");
		const FRotator SpawnRotation = WeaponMesh->GetSocketRotation("Muzzle");

		ServerSetWeaponState(EWeaponState::WS_Firing);
		
		ServerPlayFireAnim();
		ServerSpawnProjectile(SpawnLocation, SpawnRotation, this, GetCharacterOwner());

		GiveRecoil();
	}
}

void ABaseWeapon::EndFireAnim()
{
	ServerSetWeaponState(EWeaponState::WS_Ready);

	if(CurrentAmmo == 0)
	{
		Reload();
	}
}

void ABaseWeapon::Reload()
{
	if(CurrentAmmo < WeaponData.AmmoInMagazine)
	{
		ServerReload();
	}
}

void ABaseWeapon::PressedFire()
{
	switch (CurrentFireMode)
	{
	case EFireMode::FM_Auto:
		{
			Fire();
			if(!GetWorld()->GetTimerManager().IsTimerActive(TimerHandle_Fire))
			{
				GetWorld()->GetTimerManager().SetTimer(
                    TimerHandle_Fire,
                    this,
                    &ABaseWeapon::Fire,
                    WeaponData.FireDelay, 
                    true);
			}
		}
		break;
	case EFireMode::FM_Burst:
		{
				
		}
		break;
	case EFireMode::FM_Single:
		{
			Fire();
		}
		break;
	}
}

void ABaseWeapon::ReleaseFire()
{
	if(GetWorld()->GetTimerManager().IsTimerActive(TimerHandle_Fire))
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Fire);
	}
}

void ABaseWeapon::StartReload()
{
	ServerSetWeaponState(EWeaponState::WS_Reloading);
}

void ABaseWeapon::EndReload()
{
	ServerEndReload();
}


void ABaseWeapon::StartAnimShutterDistortion()
{
	if(APlayerCharacter* Player = GetCharacterOwner())
	{
		if(Player->bIsAimingRep)
		{
			Player->StopAiming();
		}
	}
	
	ServerStartAnimShutterDistortion();
}

void ABaseWeapon::StartShutterDistortion()
{
	ServerSetWeaponState(EWeaponState::WS_ShutterDistortion);
}

void ABaseWeapon::ServerStartAnimShutterDistortion_Implementation()
{	
	MultiStartAnimShutterDistortion();
}

void ABaseWeapon::MultiStartAnimShutterDistortion_Implementation()
{
	if(UAnimInstance* CharAnimInstance = GetCharacterAnimInstance())
	{
		CharAnimInstance->Montage_Play(WeaponData.CharacterShutterDistortionMontage);
	}
	
	if(UAnimInstance* AnimInstance = WeaponMesh->GetAnimInstance())
	{
		AnimInstance->Montage_Play(WeaponData.ShutterDistortionMontage);
	}
}

void ABaseWeapon::FinishShutterDistortion()
{
	if(GetCharacterOwner() && GetCharacterOwner()->IsLocallyControlled())
	{
		ServerSetWeaponState(EWeaponState::WS_Ready);
	}
}

bool ABaseWeapon::GetCanFire() const
{
	return WeaponState == EWeaponState::WS_Ready || WeaponState == EWeaponState::WS_Firing;
}

bool ABaseWeapon::GetCanAiming() const
{
	return WeaponState == EWeaponState::WS_Ready || WeaponState == EWeaponState::WS_Firing;
}


void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();

	CurrentFireMode = WeaponData.FireModes[0];

	WeaponData.FireDelay = 60.f / WeaponData.FireRate;
	
	CurrentAmmo = WeaponData.AmmoInMagazine;
}

void ABaseWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseWeapon, CurrentAmmo);
	DOREPLIFETIME(ABaseWeapon, WeaponState);
	DOREPLIFETIME(ABaseWeapon, bIsNeedShutterDistortion);
}

APlayerCharacter* ABaseWeapon::GetCharacterOwner() const
{
	return Cast<APlayerCharacter>(GetOwner());
}

void ABaseWeapon::GiveRecoil()
{
	const float VerticalRecoil = 
        FMath::RandRange(
            WeaponData.VerticalRecoil.X,
            WeaponData.VerticalRecoil.Y
            ) * -1.f;

	const float HorizontalRecoil =
        FMath::RandRange(
            WeaponData.HorizontalRecoil.X, 
            WeaponData.HorizontalRecoil.Y);

	GetCharacterOwner()->GiveRecoil(VerticalRecoil, HorizontalRecoil);
}

void ABaseWeapon::ServerSetWeaponState_Implementation(EWeaponState NewWeaponState)
{
	WeaponState = NewWeaponState;
}

UAnimInstance* ABaseWeapon::GetCharacterAnimInstance() const
{
	return Cast<ACharacter>(GetOwner())->GetMesh()->GetAnimInstance();
}

bool ABaseWeapon::CheckIsNeedReload() const
{
	return CurrentAmmo == 0;
}

void ABaseWeapon::ServerEndReload_Implementation()
{
	CurrentAmmo = WeaponData.AmmoInMagazine;

	if(!bIsNeedShutterDistortion)
	{
		ServerSetWeaponState(EWeaponState::WS_Ready);
	}
}

void ABaseWeapon::MulticastPlayFireAnim_Implementation()
{
	if(UAnimInstance* CharAnimInstance = GetCharacterAnimInstance())
	{
		CharAnimInstance->Montage_Play(WeaponData.CharFireMontage);
	}
		
	if(UAnimInstance* AnimInstance = WeaponMesh->GetAnimInstance())
	{
		AnimInstance->Montage_Play(WeaponData.FiringMontage);
	}
}

void ABaseWeapon::ServerPlayFireAnim_Implementation()
{
	MulticastPlayFireAnim();
}

void ABaseWeapon::ServerSpawnProjectile_Implementation(FVector SpawnLocation, FRotator SpawnRotator, AActor* SpawnOwner,
                                                       APawn* SpawnInstigator)
{
	if(CurrentAmmo > 0)
	{
		CurrentAmmo--;
		
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParameters.Owner = SpawnOwner;
		SpawnParameters.Instigator = SpawnInstigator;

		GetWorld()->SpawnActor<AProjectileMaster>(WeaponData.ProjectileClass, SpawnLocation, SpawnRotator, SpawnParameters);
	}
}

void ABaseWeapon::MulticastReload_Implementation()
{
	if(UAnimInstance* CharAnimInstance = GetCharacterAnimInstance())
	{
		CharAnimInstance->Montage_Play(WeaponData.CharReloadMontage);
	}
	
	if(UAnimInstance* AnimInstance = WeaponMesh->GetAnimInstance())
	{
		AnimInstance->Montage_Play(WeaponData.ReloadMontage);
	}
}

void ABaseWeapon::ServerReload_Implementation()
{
	if(CurrentAmmo < WeaponData.AmmoInMagazine)
	{	
		bIsNeedShutterDistortion = CurrentAmmo == 0 ? true : false;

		MulticastReload();
	}
}
