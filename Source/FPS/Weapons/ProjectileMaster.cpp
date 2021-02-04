// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileMaster.h"


#include "BaseWeapon.h"
#include "Components/SphereComponent.h"
#include "FPS/Player/PlayerCharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"


AProjectileMaster::AProjectileMaster()
{
	bReplicates = true;

	GravityDelay = 0.1f;
	GravityScale = 3.5f;

	ProjectileCollision = CreateDefaultSubobject<USphereComponent>(FName("ProjectileCollision"));
	ProjectileCollision->InitSphereRadius(2.f);
	ProjectileCollision->BodyInstance.SetCollisionProfileName("Projectile");
	ProjectileCollision->OnComponentHit.AddDynamic(this, &AProjectileMaster::OnProjectileHit);
	ProjectileCollision->bReturnMaterialOnMove = true;
	SetRootComponent(ProjectileCollision);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(FName("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = ProjectileCollision;
	ProjectileMovement->InitialSpeed = 40000.f;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.f;

	ProjectileParticle = CreateDefaultSubobject<UParticleSystemComponent>(FName("ProjectileParticle"));
	ProjectileParticle->SetupAttachment(RootComponent);

	InitialLifeSpan = 3.f;
}

void AProjectileMaster::OnProjectileHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	APlayerCharacter* CharRef = Cast<APlayerCharacter>(GetOwner()->GetOwner());
	
	PlayHitSound(CharRef, Hit, Hit.Location);
	
	if(HasAuthority())
	{
		if(Cast<ACharacter>(OtherActor))
		{
			FBodyPartData* BodyPartData = DT_BodyParts->FindRow<FBodyPartData>(Hit.BoneName, "");

			ABaseWeapon* OwnerWeapon = Cast<ABaseWeapon>(GetOwner());

			float Damage = 0.f;
			if(BodyPartData)
			{
				const float Distance = (StartLocation - Hit.Location).Size() / 100.f;
				Damage = OwnerWeapon->GetCalculatedDamage(BodyPartData->BodyPart, Distance);
			}
			
			UGameplayStatics::ApplyDamage(OtherActor, Damage, GetInstigatorController(), OwnerWeapon->GetOwner(), DamageTypeClass);
		}
		
		Destroy();
	}
}

void AProjectileMaster::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();

	FTimerHandle DummyHandle;
	GetWorldTimerManager().SetTimer(DummyHandle, this, &AProjectileMaster::ApplyGravity, GravityDelay);
}

void AProjectileMaster::ApplyGravity()
{
	ProjectileMovement->ProjectileGravityScale = GravityScale;
}
