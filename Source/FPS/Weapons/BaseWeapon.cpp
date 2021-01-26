// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeapon.h"

#include "Camera/CameraComponent.h"

ABaseWeapon::ABaseWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	RootComponent = WeaponMesh;

}

void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}