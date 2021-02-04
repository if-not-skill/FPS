// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "FPS/Player/PlayerCharacter.h"

#include "Engine/DataTable.h"
#include "GameFramework/Actor.h"
#include "BaseWeapon.generated.h"


UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	TW_AssaultRifle UMETA(DisplayName="AR"),
	TW_SniperRifle UMETA(DisplayName="SR"),
	TW_SubmachineGun UMETA(DisplayName="SG")
};


UENUM(BlueprintType)
enum class EFireMode : uint8
{
	FM_Single UMETA(DisplayName="Single"),
	FM_Burst UMETA(DisplayName="Burst"),
	FM_Auto UMETA(DisplayName="Auto")
};

USTRUCT(BlueprintType)
struct FWeaponData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	EWeaponType WeaponType;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	EFireMode FireMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float FireRate;
	
	float FireDelay;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float WeaponSpread;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Damage")
	float DamageMin;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Damage")
	float Damage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Damage")
	float FireRange;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Damage")
	float DamageDropMultiplier;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Damage")
	TMap<EBodyPart, float> DamageMultipliers;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Ammo")
	int AmmoMax;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Ammo")
	int AmmoInMagazine;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Recoil")
	FVector2D VerticalRecoil;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Recoil")
	FVector2D HorizontalRecoil;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Montages")
	UAnimMontage* FiringMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Montages")
	UAnimMontage* CharFireMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Montages")
	UAnimMontage* ReloadMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Montages")
	UAnimMontage* ReloadEmptyMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Montages")
	UAnimMontage* CharReloadMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Montages")
	UAnimMontage* CharReloadEmptyMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Ammo")
	TSubclassOf<class AProjectileMaster> ProjectileClass;

};


UCLASS()
class FPS_API ABaseWeapon : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName WeaponRowName;

	UPROPERTY(Replicated, BlueprintReadWrite)
	uint8 CurrentAmmo;
	
	UPROPERTY(BlueprintReadWrite)
	FWeaponData WeaponData;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Components")
	class USkeletalMeshComponent* WeaponMesh;
	
public:	
	ABaseWeapon();

	float GetCalculatedDamage(EBodyPart BodyPart, float Distance);

	void Fire();
	void Reload();
	
	UFUNCTION(Server, Reliable)
    void ServerEndReload();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UFUNCTION(Server, Reliable)
	void ServerReload();
	
	UFUNCTION(NetMulticast, Reliable)
    void MulticastReload();

	UFUNCTION(Server, Reliable)
    void ServerSpawnProjectile(FVector SpawnLocation, FRotator SpawnRotator, AActor* SpawnOwner, APawn* SpawnInstigator);

	UFUNCTION(Server, Unreliable)
    void ServerPlayFireAnim();
	
	UFUNCTION(NetMulticast, Unreliable)
    void MulticastPlayFireAnim();

	ACharacter* GetCharacterOwner() const;
	UAnimInstance* GetCharacterAnimInstance() const;

};
