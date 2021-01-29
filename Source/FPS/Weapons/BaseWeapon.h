// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

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
	float ReloadDelay;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float WeaponSpread;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float DamageMin;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float DamageMax;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int AmmoMax;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int AmmoInMagazine;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FVector2D VerticalRecoil;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FVector2D HorizontalRecoil;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UAnimMontage* FiringMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UAnimMontage* CharFireMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UAnimMontage* ReloadMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UAnimMontage* ReloadEmptyMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UAnimMontage* CharReloadMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UAnimMontage* CharReloadEmptyMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
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

	void Fire();
	void Reload();

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

	UFUNCTION(Server, Reliable)
	void ServerEndReload();
};
