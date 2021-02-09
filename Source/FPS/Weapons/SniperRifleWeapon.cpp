// Fill out your copyright notice in the Description page of Project Settings.


#include "SniperRifleWeapon.h"

ASniperRifleWeapon::ASniperRifleWeapon()
{
	bCanFire = true;
}

void ASniperRifleWeapon::Fire()
{
	if(bCanFire)
	{
		Super::Fire();
	}
	
	StartShutterDistortion();
}

void ASniperRifleWeapon::StartShutterDistortion()
{
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
