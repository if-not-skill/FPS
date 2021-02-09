// Fill out your copyright notice in the Description page of Project Settings.


#include "SniperRifleWeapon.h"


ASniperRifleWeapon::ASniperRifleWeapon()
{
	bCanFire = true;
	
	DelayShutterDistortion = 0.3f;
}

void ASniperRifleWeapon::Fire()
{
	if(bCanFire && !GetCharacterOwner()->bIsReloading)
	{
		Super::Fire();

		FTimerHandle TimerHandle;

		GetWorldTimerManager().SetTimer(TimerHandle, this, &ASniperRifleWeapon::StartShutterDistortion, DelayShutterDistortion);
	}
}

void ASniperRifleWeapon::StartShutterDistortion()
{
	if(APlayerCharacter* Player = GetCharacterOwner())
	{
		if(Player->bIsAimingRep)
		{
			Player->StopAiming();
		}
	}
	
	SetCanFire(false);
	
	ServerStartShutterDistortion();
}

void ASniperRifleWeapon::ServerStartShutterDistortion_Implementation()
{
	MultiStartShutterDistortion();
}

void ASniperRifleWeapon::MultiStartShutterDistortion_Implementation()
{
	if(UAnimInstance* CharAnimInstance = GetCharacterAnimInstance())
	{
		CharAnimInstance->Montage_Play(CharacterShutterDistortionMontage);
	}
	
	if(UAnimInstance* AnimInstance = WeaponMesh->GetAnimInstance())
	{
		AnimInstance->Montage_Play(ShutterDistortionMontage);
	}
}

void ASniperRifleWeapon::FinishShutterDistortion()
{
	SetCanFire(true);
}

void ASniperRifleWeapon::SetCanFire(bool CanFire)
{
	bCanFire = CanFire;
}

bool ASniperRifleWeapon::GetCanFire()
{
	return bCanFire;
}
