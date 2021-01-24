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
	UPROPERTY(EditDefaultsOnly, Category="Components")
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

	UPROPERTY(BlueprintReadWrite)
	class ABaseWeapon* CurrentWeapon;

	UPROPERTY(EditDefaultsOnly, Category="Spawning")
	TSubclassOf<ABaseWeapon> CurrentWeaponClass;

	UPROPERTY(EditDefaultsOnly, Category="Settings")
	float HorizontalSensitivity;
	
	UPROPERTY(EditDefaultsOnly, Category="Settings")
	float VerticalSensitivity;
	
public:
	APlayerCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	void SpawnWeapon();

	void LookUp(float Axis);
	void Turn(float Axis);

	void Aiming();

	void MoveForward(float Axis);
	void MoveRight(float Axis);

};
