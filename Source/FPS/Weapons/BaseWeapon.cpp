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
	
	return FMath::Clamp(Damage, WeaponData.DamageMin, WeaponData.Damage);
}

void ABaseWeapon::Fire()
{
	const FVector SpawnLocation = WeaponMesh->GetSocketLocation("Muzzle");
	const FRotator SpawnRotation = WeaponMesh->GetSocketRotation("Muzzle");
	
	ServerPlayFireAnim();
	ServerSpawnProjectile(SpawnLocation, SpawnRotation, this, GetCharacterOwner());
}

void ABaseWeapon::Reload()
{
	if(CurrentAmmo < WeaponData.AmmoInMagazine)
	{
		ServerReload();
	}
}

void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();

	WeaponData.FireDelay = 60.f / WeaponData.FireRate;
	
	CurrentAmmo = WeaponData.AmmoInMagazine;
}

void ABaseWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseWeapon, CurrentAmmo);
}

ACharacter* ABaseWeapon::GetCharacterOwner() const
{
	return Cast<ACharacter>(GetOwner());
}

UAnimInstance* ABaseWeapon::GetCharacterAnimInstance() const
{
	return Cast<ACharacter>(GetOwner())->GetMesh()->GetAnimInstance();
}

void ABaseWeapon::ServerEndReload_Implementation()
{
	CurrentAmmo = WeaponData.AmmoInMagazine;
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
		MulticastReload();
	}
}
