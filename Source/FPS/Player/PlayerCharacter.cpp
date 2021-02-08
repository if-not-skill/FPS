// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "Engine/DemoNetDriver.h"
#include "FPS/Weapons/BaseWeapon.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Components/DecalComponent.h"
#include "Engine/DecalActor.h"
#include "FPS/Components/HealthComponent.h"
#include "FPS/Framework/FPSGameModeBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Sound/SoundCue.h"


// Sets default values
APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	VerticalSensitivity = 1.f;
	HorizontalSensitivity = 1.f;

	DestroyDelay = 3.f;

	WalkSpeed = 150.f;
	SprintSpeed = 660.f;

	SprintFireDelay = 1.f;
	RotateCameraInterpSpeed = 1.f;

	HealthComponent = CreateDefaultSubobject<UHealthComponent>("HealthComponent");
	HealthComponent->SetIsReplicated(true);

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(GetMesh(), FName("CameraSocket"));
	Camera->bUsePawnControlRotation = true;
	
	PantsMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Pants");
	PantsMesh->SetupAttachment(GetMesh());
	PantsMesh->SetMasterPoseComponent(GetMesh());
	
	SleevesMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Sleeves");
	SleevesMesh->SetupAttachment(GetMesh());
	SleevesMesh->SetMasterPoseComponent(GetMesh());
	
	HeadMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Head");
	HeadMesh->SetupAttachment(GetMesh());
	HeadMesh->SetMasterPoseComponent(GetMesh());
	
	GlovesMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Gloves");
	GlovesMesh->SetupAttachment(GetMesh());
	GlovesMesh->SetMasterPoseComponent(GetMesh());
	
	EquipmentMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Equipment");
	EquipmentMesh->SetupAttachment(GetMesh());
	EquipmentMesh->SetMasterPoseComponent(GetMesh());
	
	FirstWeaponHingeRoot = CreateDefaultSubobject<UStaticMeshComponent>("FirstWeaponHingeRoot");
	FirstWeaponHingeRoot->SetupAttachment(GetMesh(), FName("FirstWeaponSocket"));

	FirstPhysicsSocket = CreateDefaultSubobject<UPhysicsConstraintComponent>("FirstPhysicsSocket");
	FirstPhysicsSocket->SetupAttachment(GetMesh(), FName("FirstWeaponSocket"));
	
	FirstWeaponHinge = CreateDefaultSubobject<UStaticMeshComponent>("FirstWeaponHinge");
	FirstWeaponHinge->SetupAttachment(GetMesh(), FName("FirstWeaponSocket"));
	
	SecondWeaponHingeRoot = CreateDefaultSubobject<UStaticMeshComponent>("SecondWeaponHingeRoot");
	SecondWeaponHingeRoot->SetupAttachment(GetMesh(), FName("SecondWeaponSocket"));
	
	SecondPhysicsSocket = CreateDefaultSubobject<UPhysicsConstraintComponent>("SecondPhysicsSocket");
	SecondPhysicsSocket->SetupAttachment(GetMesh(), FName("SecondWeaponSocket"));	
	
	SecondWeaponHinge = CreateDefaultSubobject<UStaticMeshComponent>("SecondWeaponHinge");
	SecondWeaponHinge->SetupAttachment(GetMesh(), FName("SecondWeaponSocket"));

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
}

void APlayerCharacter::SetWeapon()
{
	if(IsLocallyControlled())
	{
		ServerSetWeapon();
	}
}

void APlayerCharacter::StartSliding()
{
	Crouch();
}

void APlayerCharacter::EndSliding()
{
	UnCrouch();
}

void APlayerCharacter::ServerHitParticle_Implementation(UParticleSystem* ImpactParticle, FVector Location, FRotator Rotation)
{
	MultiSpawnImpactParticle(ImpactParticle, Location, Rotation);
}

void APlayerCharacter::ServerSpawnHoleDecal_Implementation(UMaterialInstance* Material, FVector Location, FRotator Rotation)
{
	MultiSpawnHoleDecal(Material, Location, Rotation);
}

void APlayerCharacter::RotateAtEnemy(AActor* Enemy)
{
	const FVector StartLocation = Camera->GetComponentLocation();
	const FVector TargetLocation = Enemy->GetTransform().GetLocation();

	const FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(StartLocation, TargetLocation);
	const FRotator CurrentRotation = Camera->GetComponentRotation();

	const FRotator NewRotation = UKismetMathLibrary::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), RotateCameraInterpSpeed);
	
	GetWorld()->GetFirstPlayerController()->SetControlRotation(NewRotation);
}

void APlayerCharacter::HighlightingEnemy(AActor* Enemy)
{
	ClientHighlightingEnemy(Enemy);
	
	MyKiller = Enemy;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	SpawnWeapon();
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	
	bIsDie = false;
}

void APlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(IsLocallyControlled())
	{
		ServerSetControlRotationRep();
	}
	
	CheckSprintDirection();
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("Turn", this, &APlayerCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharacter::LookUp);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);

	PlayerInputComponent->BindAction("Aiming", IE_Pressed, this, &APlayerCharacter::StartAiming);
	PlayerInputComponent->BindAction("Aiming", IE_Released, this, &APlayerCharacter::StopAiming);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacter::StartJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &APlayerCharacter::StopJump);
	
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &APlayerCharacter::StartSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &APlayerCharacter::StopSprint);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &APlayerCharacter::StartCrouch);
	
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APlayerCharacter::PressedFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &APlayerCharacter::ReleasedFire);
	
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &APlayerCharacter::Reload);
	
	PlayerInputComponent->BindAction("SwitchWeapon", IE_Pressed, this, &APlayerCharacter::SwitchWeapon);
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerCharacter, ControlRotationRep);
	DOREPLIFETIME(APlayerCharacter, bIsSprinting);
	DOREPLIFETIME(APlayerCharacter, bIsAimingRep);
	DOREPLIFETIME(APlayerCharacter, FirstWeapon);
	DOREPLIFETIME(APlayerCharacter, SecondWeapon);
	DOREPLIFETIME(APlayerCharacter, CurrentWeapon);
	DOREPLIFETIME(APlayerCharacter, MyKiller);
}


void APlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if(FirstWeapon)
	{
		FirstWeapon->Destroy();
	}
		
	if(SecondWeapon)
	{
		SecondWeapon->Destroy();
	}

	if(GetWorldTimerManager().IsTimerActive(TimerHandle_UpdateRotation))
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_UpdateRotation);
	}
}

void APlayerCharacter::SpawnWeapon()
{
	if(HasAuthority())
	{
		ServerSpawnWeapon();
	}
}

void APlayerCharacter::ServerSpawnWeapon_Implementation()
{
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	
	FirstWeapon = GetWorld()->SpawnActor<ABaseWeapon>(FirstWeaponClass, FTransform(), SpawnParameters);
	FirstWeapon->AttachToComponent
    (
        GetMesh(),
        FAttachmentTransformRules::KeepRelativeTransform,
        FName("RightHandSocket")
    );

	SecondWeapon = GetWorld()->SpawnActor<ABaseWeapon>(SecondWeaponClass, FTransform(), SpawnParameters);
	SecondWeapon->AttachToComponent
    (
        SecondWeaponHinge,
        FAttachmentTransformRules::SnapToTargetNotIncludingScale
    );

	CurrentWeapon = FirstWeapon;
}

void APlayerCharacter::LookUp(float Axis)
{
	LookValue = Axis * VerticalSensitivity * GetWorld()->GetDeltaSeconds();
	
	AddControllerPitchInput(LookValue);
}

void APlayerCharacter::Turn(float Axis)
{
	TurnValue = Axis * HorizontalSensitivity * GetWorld()->GetDeltaSeconds();
	
	AddControllerYawInput(TurnValue);
}

void APlayerCharacter::MoveForward(float Axis)
{
	MoveForwardValue = Axis;
	
	if(MoveForwardValue != 0.f)
	{
		AddMovementInput(GetActorForwardVector(), Axis);
	}
}

void APlayerCharacter::MoveRight(float Axis)
{
	MoveRightValue = Axis;
	
	if(MoveRightValue != 0.f)
	{
		AddMovementInput(GetActorRightVector(), Axis);
	}
}

void APlayerCharacter::StartJump()
{
	Jump();
}

void APlayerCharacter::StopJump()
{
	StopJumping();
}

void APlayerCharacter::StartSprint()
{
	if(MoveForwardValue < 0.f) return;
	
	if(!HasAuthority())
	{
		if(GetCharacterMovement()->IsCrouching()) UnCrouch();
		if(bIsAimingRep) StopAiming();
		
		ServerStartSprint();
	}

	bIsSprinting = true;
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void APlayerCharacter::StopSprint()
{
	if(!HasAuthority())
	{
		ServerStopSprint();
	}

	bIsSprinting = false;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void APlayerCharacter::StartAiming()
{
	if(!bIsReloading)
	{
		if(bIsSprinting) StopSprint();

		ServerStartAiming();
		ToggleADS(true);
	}
}

void APlayerCharacter::StopAiming()
{
	ServerStopAiming();
	ToggleADS(false);
}

void APlayerCharacter::ServerStartAiming_Implementation()
{
	bIsAimingRep = true;
}

void APlayerCharacter::ServerStopAiming_Implementation()
{
	bIsAimingRep = false;
}

void APlayerCharacter::StartCrouch()
{
	if(GetCharacterMovement()->IsCrouching())
	{
		UnCrouch();
	}
	else
	{
		const float Speed = GetVelocity().Size();
		const float ForwardVelocity = FVector::DotProduct(GetVelocity(), GetActorForwardVector());
		
		if(ForwardVelocity > 0.f && Speed > WalkSpeed + 80.f) 
		{
			Slide();
			return;
		}
		
		Crouch();
	}
}

void APlayerCharacter::HandleCameraShakeHit()
{
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->ClientStartCameraShake(CameraShakeHit);
}

void APlayerCharacter::CheckSprintDirection()
{
	if(!bIsSprinting) return;

	if(MoveForwardValue < 0.f)
	{
		StopSprint();
	}
}

void APlayerCharacter::PressedFire()
{
	if(!CurrentWeapon) return;

	if(GetWorld()->GetTimerManager().IsTimerActive(TimerHandle_SprintFireDelay))
	{
		return;
	}
	
	if(bIsSprinting)
	{
		StopSprint();
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_SprintFireDelay, this, &APlayerCharacter::ClearSprintFireTimer, SprintFireDelay);
		return;
	}
	
	switch (CurrentWeapon->WeaponData.FireMode)
	{
		case EFireMode::FM_Auto:
			{
				Fire();
				if(!GetWorld()->GetTimerManager().IsTimerActive(TimerHandle_Fire))
				{
					GetWorld()->GetTimerManager().SetTimer(
						TimerHandle_Fire,
						this,
						&APlayerCharacter::Fire,
						CurrentWeapon->WeaponData.FireDelay, 
						true);
				}
			}
			break;
		case EFireMode::FM_Burst:
			{
				
			}
			break;
		case EFireMode::FM_Single:
			{
				Fire();
			}
			break;
	}
}

void APlayerCharacter::ReleasedFire()
{
	if(GetWorld()->GetTimerManager().IsTimerActive(TimerHandle_Fire))
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Fire);
	}
}

void APlayerCharacter::Fire()
{
	if(CurrentWeapon && !bIsReloading && !bIsDie)
	{
		if(CurrentWeapon->CurrentAmmo > 0)
		{
			CurrentWeapon->Fire();
			GiveRecoil();
		}
		else
		{
			if(bIsAimingRep)
			{
				StopAiming();
			}
			
			Reload();
		}
	}
}

void APlayerCharacter::Reload()
{
	if(!bIsReloading)
	{
		CurrentWeapon->Reload();
	}
}

void APlayerCharacter::GiveRecoil()
{
	const float VerticalRecoil = 
        FMath::RandRange(
            CurrentWeapon->WeaponData.VerticalRecoil.X,
            CurrentWeapon->WeaponData.VerticalRecoil.Y
            ) * -1.f;

	const float HorizontalRecoil =
        FMath::RandRange(
            CurrentWeapon->WeaponData.HorizontalRecoil.X, 
            CurrentWeapon->WeaponData.HorizontalRecoil.Y);

	LookUp(VerticalRecoil);
	Turn(HorizontalRecoil);	
}

void APlayerCharacter::Slide()
{
	ServerSlide();
}

void APlayerCharacter::MultiSlide_Implementation()
{
	if(UAnimInstance* CharAnimInstance = GetMesh()->GetAnimInstance())
	{
		CharAnimInstance->Montage_Play(SlideMontage);
	}
}

void APlayerCharacter::ServerSlide_Implementation()
{
	MultiSlide();
}

void APlayerCharacter::ShowHitVisual_Implementation(FVector HitLocation)
{
	HitVisual(HitLocation);
}

void APlayerCharacter::Die()
{
	bIsDie = true;
	
	if(GetLocalRole() == ROLE_Authority)
	{
		HealthComponent->DestroyComponent();
		
		MultiDie();

		AGameModeBase* GM = GetWorld()->GetAuthGameMode();
		if(AFPSGameModeBase* GameMode = Cast<AFPSGameModeBase>(GM))
		{
			GameMode->Respawn(GetController());
		}
		
		FTimerHandle TimerHandle_Destroy;
		GetWorldTimerManager().SetTimer(TimerHandle_Destroy, this, &APlayerCharacter::CallDestroy, DestroyDelay);
	}

	DeleteHUD();
}

void APlayerCharacter::LookAtEnemy(AActor* Enemy)
{
	ClientLookAtEnemy(Enemy);
}

void APlayerCharacter::StartReload()
{
	if(IsLocallyControlled())
	{
		bIsReloading = true;
		
		if(bIsAimingRep)
		{
			StopAiming();
		}
	}
}

void APlayerCharacter::EndReload()
{
	if(IsLocallyControlled())
	{
		bIsReloading = false;
		CurrentWeapon->ServerEndReload();
	}
}

void APlayerCharacter::ServerHitAudio_Implementation(USoundCue* HitSound, FVector Location)
{
	MultiHitAudio(HitSound, Location);
}

void APlayerCharacter::MultiDie_Implementation()
{
	if(IsLocallyControlled())
	{
		GetCharacterMovement()->DisableMovement();
		
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		DisableInput(PlayerController);
		
		Camera->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	}
	
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionProfileName("Ragdoll");
	GetMesh()->SetAllBodiesSimulatePhysics(true);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void APlayerCharacter::CallDestroy()
{
	if(GetLocalRole() == ROLE_Authority)
	{
		ClientDisableHighlightingEnemy(MyKiller);

		Destroy();
	}
}

void APlayerCharacter::SwitchWeapon()
{
	if(IsLocallyControlled())
	{
		ServerSwitchWeapon();
	}
}

void APlayerCharacter::ClearSprintFireTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_SprintFireDelay);
	PressedFire();
}

void APlayerCharacter::ClientDisableHighlightingEnemy_Implementation(AActor* Enemy)
{
	TArray<UActorComponent*> VisualComponents;
	Enemy->GetComponents(UPrimitiveComponent::StaticClass(), VisualComponents);
	
	for(auto& VisualComp : VisualComponents)
	{
		if(UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(VisualComp))
		{
			Prim->SetRenderCustomDepth(false);
		}
	}
}

void APlayerCharacter::ClientHighlightingEnemy_Implementation(AActor* Enemy)
{
	TArray<UActorComponent*> VisualComponents;
	Enemy->GetComponents(UPrimitiveComponent::StaticClass(), VisualComponents);
	
	for(auto& VisualComp : VisualComponents)
	{
		if(UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(VisualComp))
		{
			Prim->SetRenderCustomDepth(true);
		}
	}
}


void APlayerCharacter::ClientLookAtEnemy_Implementation(AActor* Enemy)
{
	FTimerDelegate TimerDel;
 
	TimerDel.BindUFunction(this, FName("RotateAtEnemy"), Enemy);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_UpdateRotation, TimerDel, 0.01f, true);
}

void APlayerCharacter::ServerSetWeapon_Implementation()
{
	if(CurrentWeapon)
	{
		if(CurrentWeapon == FirstWeapon)
		{
			SecondWeapon->AttachToComponent
            (
                GetMesh(),
                FAttachmentTransformRules::SnapToTargetNotIncludingScale,
                FName("RightHandSocket")
            );

			FirstWeapon->AttachToComponent
            (
                FirstWeaponHinge,
                FAttachmentTransformRules::SnapToTargetNotIncludingScale
            );

			CurrentWeapon = SecondWeapon;
		}
		else if(CurrentWeapon == SecondWeapon)
		{
			FirstWeapon->AttachToComponent
            (
                GetMesh(),
                FAttachmentTransformRules::SnapToTargetNotIncludingScale,
                FName("RightHandSocket")
            );

			SecondWeapon->AttachToComponent
            (
                SecondWeaponHinge,
                FAttachmentTransformRules::SnapToTargetNotIncludingScale
            );

			CurrentWeapon = FirstWeapon;
		}
	}
}

void APlayerCharacter::MultiSwitchWeapon_Implementation()
{
	if(UAnimInstance* AnimInstance = Cast<UAnimInstance>(GetMesh()->GetAnimInstance()))
	{
		AnimInstance->Montage_Play(SwitchWeaponMontage);
	}
}

void APlayerCharacter::ServerSwitchWeapon_Implementation()
{
	MultiSwitchWeapon();
}

void APlayerCharacter::MultiSpawnHoleDecal_Implementation(UMaterialInstance* Material, FVector Location, FRotator Rotation)
{
	ADecalActor* decal = GetWorld()->SpawnActor<ADecalActor>(Location, Rotation);
	if (decal)
	{
		decal->SetDecalMaterial(Material);
		decal->SetLifeSpan(30.0f);
		decal->GetDecal()->SetFadeScreenSize(0.001f);
		decal->GetDecal()->DecalSize = FVector(10.f, 10.f, 10.f);
	}
}

void APlayerCharacter::MultiSpawnImpactParticle_Implementation(UParticleSystem* ImpactParticle, FVector Location, FRotator Rotation)
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, Location, Rotation);
}

void APlayerCharacter::MultiHitAudio_Implementation(USoundCue* HitSound, FVector Location)
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, Location);
}

void APlayerCharacter::ServerSetControlRotationRep_Implementation()
{
	if(IsLocallyControlled() || HasAuthority())
	{
		ControlRotationRep = GetControlRotation();
	}
}

void APlayerCharacter::ServerStopSprint_Implementation()
{
	StopSprint();
}

void APlayerCharacter::ServerStartSprint_Implementation()
{
	StartSprint();
}
