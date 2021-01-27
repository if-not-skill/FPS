// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"


#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "Engine/DemoNetDriver.h"
#include "FPS/Weapons/BaseWeapon.h"
#include "FPS/Weapons/ProjectileMaster.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	VerticalSensitivity = 1.f;
	HorizontalSensitivity = 1.f;

	WalkSpeed = 150.f;
	SprintSpeed = 660.f;

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

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
}



void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	SpawnWeapon();
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	
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

}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerCharacter, ControlRotationRep);
	DOREPLIFETIME(APlayerCharacter, bIsSprinting);
	DOREPLIFETIME(APlayerCharacter, bIsAiming);
	DOREPLIFETIME(APlayerCharacter, CurrentWeapon);
}

void APlayerCharacter::SpawnWeapon()
{	
	FActorSpawnParameters SpawnParameters;
	
	CurrentWeapon = GetWorld()->SpawnActor<ABaseWeapon>(CurrentWeaponClass, FTransform(), SpawnParameters);
	CurrentWeapon->AttachToComponent
	(
		GetMesh(),
		FAttachmentTransformRules::KeepRelativeTransform,
		FName("RightHandSocket")
	);
	CurrentWeapon->SetOwner(this);
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
		HandleCameraShake();
	}
}

void APlayerCharacter::MoveRight(float Axis)
{
	MoveRightValue = Axis;
	
	if(MoveRightValue != 0.f)
	{
		AddMovementInput(GetActorRightVector(), Axis);
		HandleCameraShake();
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
		if(GetCharacterMovement()->IsCrouching())
		{
			UnCrouch();
		}
		
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
	if(!HasAuthority())
	{
		ServerStartAiming();
	}
	
	ToggleADS();
}

void APlayerCharacter::StopAiming()
{
	if(!HasAuthority())
	{
		ServerStopAiming();
	}

	ToggleADS();
}

void APlayerCharacter::ServerStartAiming_Implementation()
{
	bIsAiming = true;
}

void APlayerCharacter::ServerStopAiming_Implementation()
{
	bIsAiming = false;
}

void APlayerCharacter::StartCrouch()
{
	if(GetCharacterMovement()->IsCrouching())
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

void APlayerCharacter::HandleCameraShake()
{
	if(bIsSprinting)
	{
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->ClientStartCameraShake(CameraShakeSprint);	
	}
	else
	{
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->ClientStartCameraShake(CameraShakeWalk);
	}
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
						CurrentWeapon->WeaponData.FireRate, 
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
	if(CurrentWeapon)
	{
		FVector SpawnLocation = CurrentWeapon->WeaponMesh->GetSocketLocation("Muzzle");
		FRotator SpawnRotation = CurrentWeapon->WeaponMesh->GetSocketRotation("Muzzle");
		ServerSpawnProjectile(SpawnLocation, SpawnRotation, CurrentWeapon, this);
		ServerSpawnFireEffects();
	}
}

void APlayerCharacter::ServerSpawnFireEffects_Implementation()
{
	MulticastSpawnFireEffects();
}

void APlayerCharacter::MulticastSpawnFireEffects_Implementation()
{
	UGameplayStatics::SpawnEmitterAttached(
        CurrentWeapon->WeaponData.ParticleMuzzleFlash,
        CurrentWeapon->WeaponMesh,
        FName("Muzzle"),
        FVector::ZeroVector,
        FRotator::ZeroRotator,
        EAttachLocation::SnapToTarget,
        true);
	
	UGameplayStatics::SpawnEmitterAttached(
        CurrentWeapon->WeaponData.ParticleShellEject,
        CurrentWeapon->WeaponMesh,
        FName("ShellEjection"),
        FVector::ZeroVector,
        FRotator::ZeroRotator,
        EAttachLocation::SnapToTarget,
        true);
}

void APlayerCharacter::ServerSpawnProjectile_Implementation(FVector SpawnLocation, FRotator SpawnRotator, AActor* SpawnOwner, APawn* SpawnInstigator)
{
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParameters.Owner = SpawnOwner;
	SpawnParameters.Instigator = SpawnInstigator;

	GetWorld()->SpawnActor<AProjectileMaster>(CurrentWeapon->WeaponData.ProjectileClass, SpawnLocation, SpawnRotator, SpawnParameters);
	
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
