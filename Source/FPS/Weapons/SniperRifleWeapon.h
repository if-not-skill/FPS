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
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* ShutterDistortionMontage;
	
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* CharacterShutterDistortionMontage;

	UPROPERTY(EditDefaultsOnly)
	float DelayShutterDistortion;

protected:
	UPROPERTY(BlueprintReadWrite)
	bool bCanFire;
	
public:
	ASniperRifleWeapon();

	virtual void Fire() override;

	UFUNCTION(BlueprintCallable)
	void StartShutterDistortion();
	
	UFUNCTION(Server, Reliable)
    void ServerStartShutterDistortion();
	
	UFUNCTION(NetMulticast, Reliable)
    void MultiStartShutterDistortion();

	UFUNCTION(BlueprintCallable)
	void FinishShutterDistortion();

	UFUNCTION(BlueprintCallable)
	void SetCanFire(bool CanFire);
	bool GetCanFire();
	
};
