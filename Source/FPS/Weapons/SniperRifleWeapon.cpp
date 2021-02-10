// Fill out your copyright notice in the Description page of Project Settings.


#include "SniperRifleWeapon.h"


void ASniperRifleWeapon::Fire()
{
	if(WeaponState == EWeaponState::WS_Ready)
	{
		Super::Fire();

		if(GetCharacterOwner()->bIsAimingRep)
		{
			GetCharacterOwner()->StopAiming();
		}
	}
}

void ASniperRifleWeapon::EndFireAnim()
{
	if(CurrentAmmo == 0)
	{
		Reload();
	}
	else
	{
		StartAnimShutterDistortion();
	}
}

bool ASniperRifleWeapon::GetCanAiming() const
{
	return WeaponState == EWeaponState::WS_Ready;
}
