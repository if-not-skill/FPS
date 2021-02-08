// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "Engine/DataTable.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"


UENUM(BlueprintType)
enum class EBodyPart : uint8
{
	BP_Body UMETA(DisplayName="Body"),
	BP_Legs UMETA(DisplayName="Legs"),
	BP_Hands UMETA(DisplayName="Hands"),
	BP_Head UMETA(DisplayName="Head"),
};

USTRUCT(BlueprintType)
struct FBodyPartData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	EBodyPart BodyPart;
};


UCLASS()
class FPS_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	class UCameraComponent* Camera;
	
	UPROPERTY(EditDefaultsOnly, Category="Components")
	class USkeletalMeshComponent* PantsMesh;
	
	UPROPERTY(EditDefaultsOnly, Category="Components")
	class USkeletalMeshComponent* SleevesMesh;
	
	UPROPERTY(EditDefaultsOnly, Category="Components")
	class USkeletalMeshComponent* HeadMesh;
	
	UPROPERTY(EditDefaultsOnly, Category="Components")
	class USkeletalMeshComponent* GlovesMesh;
	
	UPROPERTY(EditDefaultsOnly, Category="Components")
	class USkeletalMeshComponent* EquipmentMesh;

	UPROPERTY(EditDefaultsOnly, Category="Components")
	class UStaticMeshComponent* FirstWeaponHingeRoot;
	
	UPROPERTY(EditDefaultsOnly, Category="Components")
	class UStaticMeshComponent* SecondWeaponHingeRoot;
	
	UPROPERTY(EditDefaultsOnly, Category="Components")
	class UStaticMeshComponent* FirstWeaponHinge;
	
	UPROPERTY(EditDefaultsOnly, Category="Components")
	class UStaticMeshComponent* SecondWeaponHinge;
	
	UPROPERTY(EditDefaultsOnly, Category="Components")
	class UPhysicsConstraintComponent* SecondPhysicsSocket;
	
	UPROPERTY(EditDefaultsOnly, Category="Components")
	class UPhysicsConstraintComponent* FirstPhysicsSocket;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	class UHealthComponent* HealthComponent;

	UPROPERTY(BlueprintReadWrite, Replicated)
	class ABaseWeapon* FirstWeapon;
	
	UPROPERTY(BlueprintReadWrite, Replicated)
	class ABaseWeapon* SecondWeapon;

	UPROPERTY(BlueprintReadOnly, Replicated)
	class ABaseWeapon* CurrentWeapon;

	UPROPERTY(EditDefaultsOnly)
	class UAnimMontage* SwitchWeaponMontage;
	
	UPROPERTY(EditDefaultsOnly, Category="Spawning")
	TSubclassOf<ABaseWeapon> FirstWeaponClass;

	UPROPERTY(EditDefaultsOnly, Category="Spawning")
	TSubclassOf<ABaseWeapon> SecondWeaponClass;
	
	UPROPERTY(EditDefaultsOnly, Category="Settings")
	float HorizontalSensitivity;
	
	UPROPERTY(EditDefaultsOnly, Category="Settings")
	float VerticalSensitivity;

	UPROPERTY(Replicated, BlueprintReadOnly)
	FRotator ControlRotationRep;

	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bIsSprinting;

	UPROPERTY(BlueprintReadOnly)
	bool bIsDie;
	
	UPROPERTY(Replicated, BlueprintReadWrite)
	bool bIsAimingRep;

	UPROPERTY(BlueprintReadWrite)
	bool bIsReloading;

	UPROPERTY(BlueprintReadWrite)
	float MoveForwardValue;
	
	UPROPERTY(BlueprintReadWrite)
	float MoveRightValue;
	
	UPROPERTY(BlueprintReadWrite)
	float TurnValue;
	
	UPROPERTY(BlueprintReadWrite)
	float LookValue;

	UPROPERTY(EditDefaultsOnly)
	float DestroyDelay;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* SlideMontage;

	UPROPERTY(EditDefaultsOnly, Category="CameraShake")
	TSubclassOf<UCameraShakeBase> CameraShakeHit;

	UPROPERTY(EditDefaultsOnly, Category="Settings")
	float SprintFireDelay;

	UPROPERTY(EditDefaultsOnly, Category="Settings")
	float RotateCameraInterpSpeed;

private:
	UPROPERTY(EditDefaultsOnly, Category="CharacterSettings")
	float WalkSpeed;
	
	UPROPERTY(EditDefaultsOnly, Category="CharacterSettings")
	float SprintSpeed;

	FTimerHandle TimerHandle_Fire;
	FTimerHandle TimerHandle_SprintFireDelay;
	FTimerHandle TimerHandle_UpdateRotation;

	UPROPERTY(Replicated)
	AActor* MyKiller;
	
public:
	APlayerCharacter();

	UFUNCTION(BlueprintCallable)
	void SetWeapon();
	
	UFUNCTION(BlueprintImplementableEvent)
	void ToggleADS(bool IsAiming);

	UFUNCTION(BlueprintCallable)
	void StartSliding();
	
	UFUNCTION(BlueprintCallable)
    void EndSliding();

	UFUNCTION(BlueprintImplementableEvent)
	void HitVisual(FVector HitLocation);

	UFUNCTION(Client, Reliable)
	void ShowHitVisual(FVector HitLocation);
	
	void Die();
	void LookAtEnemy(AActor* Enemy);

	UFUNCTION(BlueprintCallable)
	void StartReload();
	
	UFUNCTION(BlueprintCallable)
    void EndReload();

	UFUNCTION(Server, Unreliable, BlueprintCallable)
	void ServerHitAudio(class USoundCue* HitSound, FVector Location);

	UFUNCTION(Server, Unreliable, BlueprintCallable)
	void ServerHitParticle(UParticleSystem* ImpactParticle, FVector Location, FRotator Rotation);

	UFUNCTION(Server, Unreliable, BlueprintCallable)
	void ServerSpawnHoleDecal(UMaterialInstance* Material, FVector Location, FRotator Rotation);
	
	UFUNCTION(Server, Reliable)
    void ServerSpawnWeapon();
	
	void HandleCameraShakeHit();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateHealthBar();
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void CreateHUD();
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void DeleteHUD();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void ShowHitMarker(bool EnemyIsAlive);

	UFUNCTION()
    void RotateAtEnemy(AActor* Enemy);

	void HighlightingEnemy(AActor* Enemy);
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
private:
	void SpawnWeapon();

	void LookUp(float Axis);
	void Turn(float Axis);

	void MoveForward(float Axis);
	void MoveRight(float Axis);

	void StartJump();
	void StopJump();

	void StartSprint();
	void StopSprint();

	UFUNCTION(Server, Reliable)
	void ServerStartSprint();
	
	UFUNCTION(Server, Reliable)
    void ServerStopSprint();
	
	void StartAiming();
	void StopAiming();
	
	UFUNCTION(Server, Reliable)
    void ServerStartAiming();
	
	UFUNCTION(Server, Reliable)
    void ServerStopAiming();

	void StartCrouch();

	UFUNCTION(Server, Reliable)
	void ServerSetControlRotationRep();

	void CheckSprintDirection();

	void PressedFire();
	void ReleasedFire();
	
	void Fire();
	void Reload();
	void GiveRecoil();

	void Slide();

	UFUNCTION(Server, Reliable)
	void ServerSlide();

	UFUNCTION(NetMulticast, Reliable)
	void MultiSlide();

	UFUNCTION(NetMulticast, Reliable)
    void MultiDie();
	
	void CallDestroy();

	UFUNCTION(NetMulticast, Unreliable)
	void MultiHitAudio(class USoundCue* HitSound, FVector Location);
	
	UFUNCTION(NetMulticast, Unreliable)
    void MultiSpawnImpactParticle(UParticleSystem* ImpactParticle, FVector Location, FRotator Rotation);
	
	UFUNCTION(NetMulticast, Unreliable, BlueprintCallable)
    void MultiSpawnHoleDecal(UMaterialInstance* Material, FVector Location, FRotator Rotation);

	void SwitchWeapon();

	UFUNCTION(Server, Reliable)
	void ServerSwitchWeapon();
	
	UFUNCTION(NetMulticast, Reliable)
    void MultiSwitchWeapon();

	UFUNCTION(Server, Reliable)
	void ServerSetWeapon();

	UFUNCTION()
	void ClearSprintFireTimer();

	UFUNCTION(Client, Unreliable)
	void ClientLookAtEnemy(AActor* Enemy);
	
	UFUNCTION(Client, Unreliable)
    void ClientHighlightingEnemy(AActor* Enemy);

	UFUNCTION(Client, Reliable)
	void ClientDisableHighlightingEnemy(AActor* Enemy);

};
