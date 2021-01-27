// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

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

	UPROPERTY(BlueprintReadWrite, Replicated)
	class ABaseWeapon* CurrentWeapon;

	UPROPERTY(EditDefaultsOnly, Category="Spawning")
	TSubclassOf<ABaseWeapon> CurrentWeaponClass;

	UPROPERTY(EditDefaultsOnly, Category="Settings")
	float HorizontalSensitivity;
	
	UPROPERTY(EditDefaultsOnly, Category="Settings")
	float VerticalSensitivity;

	UPROPERTY(Replicated, BlueprintReadOnly)
	FRotator ControlRotationRep;

	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bIsSprinting;
	
	UPROPERTY(Replicated, BlueprintReadWrite)
	bool bIsAiming;

	UPROPERTY(BlueprintReadWrite)
	float MoveForwardValue;
	
	UPROPERTY(BlueprintReadWrite)
	float MoveRightValue;
	
	UPROPERTY(BlueprintReadWrite)
	float TurnValue;
	
	UPROPERTY(BlueprintReadWrite)
	float LookValue;

	UPROPERTY(EditDefaultsOnly, Category="CameraShake")
	TSubclassOf<UCameraShakeBase> CameraShakeWalk;
	
	UPROPERTY(EditDefaultsOnly, Category="CameraShake")
	TSubclassOf<UCameraShakeBase> CameraShakeSprint;

private:
	UPROPERTY(EditDefaultsOnly, Category="CharacterSettings")
	float WalkSpeed;
	
	UPROPERTY(EditDefaultsOnly, Category="CharacterSettings")
	float SprintSpeed;

	FTimerHandle TimerHandle_Fire;
	
public:
	APlayerCharacter();
	
	UFUNCTION(BlueprintImplementableEvent)
	void ToggleADS();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

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

	void HandleCameraShake();

	void CheckSprintDirection();

	void PressedFire();
	void ReleasedFire();
	
	void Fire();

	UFUNCTION(Server, Reliable)
	void ServerSpawnProjectile(FVector SpawnLocation, FRotator SpawnRotator, AActor* SpawnOwner, APawn* SpawnInstigator);

	UFUNCTION(Server, Unreliable)
    void ServerSpawnFireEffects();
	
	UFUNCTION(NetMulticast, Unreliable)
    void MulticastSpawnFireEffects();
};
