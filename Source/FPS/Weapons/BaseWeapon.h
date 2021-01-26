// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseWeapon.generated.h"

UCLASS()
class FPS_API ABaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	ABaseWeapon();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Components")
	class USkeletalMeshComponent* WeaponMesh;

protected:
	virtual void BeginPlay() override;

};
