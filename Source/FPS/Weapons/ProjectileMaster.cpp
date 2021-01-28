// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileMaster.h"


#include "BaseWeapon.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"

AProjectileMaster::AProjectileMaster()
{
	bReplicates = true;

	GravityDelay = 0.1f;
	GravityScale = 3.5f;

	ProjectileCollision = CreateDefaultSubobject<UBoxComponent>(FName("ProjectileCollision"));
	ProjectileCollision->InitBoxExtent(FVector(2.f));
	ProjectileCollision->BodyInstance.SetCollisionProfileName("Projectile");
	ProjectileCollision->OnComponentHit.AddDynamic(this, &AProjectileMaster::OnProjectileHit);
	SetRootComponent(ProjectileCollision);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(FName("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = ProjectileCollision;
	ProjectileMovement->InitialSpeed = 40000.f;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.f;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("ProjectileMesh"));
	ProjectileMesh->SetupAttachment(RootComponent);

	ProjectileParticle = CreateDefaultSubobject<UParticleSystemComponent>(FName("ProjectileParticle"));
	ProjectileParticle->SetupAttachment(RootComponent);

	InitialLifeSpan = 3.f;
}

void AProjectileMaster::OnProjectileHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if(HasAuthority())
	{
	
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Hit: %s"), *OtherActor->GetName()));
		
		Destroy();
	}
}

void AProjectileMaster::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle DummyHandle;
	GetWorldTimerManager().SetTimer(DummyHandle, this, &AProjectileMaster::ApplyGravity, GravityDelay);
}

void AProjectileMaster::ApplyGravity()
{
	ProjectileMovement->ProjectileGravityScale = GravityScale;
}
