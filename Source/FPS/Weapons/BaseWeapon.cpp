// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeapon.h"

#include "Camera/CameraComponent.h"

ABaseWeapon::ABaseWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	RootComponent = WeaponMesh;

	WeaponCamera = CreateDefaultSubobject<UCameraComponent>("Camera");
	WeaponCamera->SetupAttachment(WeaponMesh, FName("WCameraSocket"));

}

void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}