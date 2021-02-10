// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "SniperRifleWeapon.generated.h"

/**
 * 
 */
UCLASS()
class FPS_API ASniperRifleWeapon : public ABaseWeapon
{
	GENERATED_BODY()

public:
	virtual void Fire() override;
	virtual void EndFireAnim() override;
	virtual bool GetCanAiming() const override;
	
};
