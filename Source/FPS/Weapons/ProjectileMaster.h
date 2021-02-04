// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "ProjectileMaster.generated.h"

UCLASS()
class FPS_API AProjectileMaster : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class USphereComponent* ProjectileCollision;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UParticleSystemComponent* ProjectileParticle;	
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float GravityDelay;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float GravityScale;

	UPROPERTY(EditDefaultsOnly)
	class UDataTable* DT_BodyParts;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageType> DamageTypeClass;

private:
	FVector StartLocation;
	
public:	
	AProjectileMaster();

	UFUNCTION(BlueprintImplementableEvent)
	void PlayHitSound(class APlayerCharacter* CharRef, const FHitResult& Hit, FVector HitLocation);
	
	UFUNCTION()
	void OnProjectileHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
protected:
	virtual void BeginPlay() override;

private:
	void ApplyGravity();

};
