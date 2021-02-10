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
	TW_AssaultRifle UMETA(DisplayName="AssaultRifle"),
	TW_SniperRifle UMETA(DisplayName="SniperRifle"),
	TW_SubmachineGun UMETA(DisplayName="SubmachineGun")
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
	TArray<EFireMode> FireModes;

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
	UAnimMontage* CharReloadMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Montages")
	UAnimMontage* ShutterDistortionMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Montages")
	UAnimMontage* CharacterShutterDistortionMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Ammo")
	TSubclassOf<class AProjectileMaster> ProjectileClass;

};

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	WS_Empty UMETA(DisplayName="Empty"),
	WS_Reloading UMETA(DisplayName="Reloading"),
	WS_ShutterDistortion UMETA(DisplayName="ShutterDistortion"),
	WS_Firing UMETA(DisplayName="Firing"),
	WS_Ready UMETA(DisplayName="Ready"),
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

	UPROPERTY(BlueprintReadOnly, Replicated)
	EWeaponState WeaponState;
	
	UPROPERTY(BlueprintReadOnly, Replicated)
	bool bIsNeedShutterDistortion;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<class AScopeBase> WeaponDefaultScopeClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Replicated)
	class AScopeBase* WeaponScope;

private:
	FTimerHandle TimerHandle_Fire;

	EFireMode CurrentFireMode;
	
public:	
	ABaseWeapon();

	float GetCalculatedDamage(EBodyPart BodyPart, float Distance);

	virtual void Fire();

	UFUNCTION(BlueprintCallable)
	virtual void EndFireAnim();
	
	void Reload();

	void PressedFire();
	void ReleaseFire();

	void StartReload();
	void EndReload();
	
	UFUNCTION(BlueprintCallable)
    void StartAnimShutterDistortion();
	
	UFUNCTION(BlueprintCallable)
    void StartShutterDistortion();
	
	UFUNCTION(Server, Reliable)
    void ServerStartAnimShutterDistortion();
	
	UFUNCTION(NetMulticast, Reliable)
    void MultiStartAnimShutterDistortion();

	UFUNCTION(BlueprintCallable)
    void FinishShutterDistortion();

	bool GetCanFire() const;
	virtual bool GetCanAiming() const;

	void SpawnScope();

	UFUNCTION(Server, Reliable)
	void ServerSpawnScope(TSubclassOf<AScopeBase> ScopeClass);

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	bool CheckIsNeedReload() const;
	UAnimInstance* GetCharacterAnimInstance() const;
	APlayerCharacter* GetCharacterOwner() const;
	
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

	void GiveRecoil();

	UFUNCTION(Server, Reliable)
	void ServerSetWeaponState(EWeaponState NewWeaponState);
	
	UFUNCTION(Server, Reliable)
    void ServerEndReload();

};
