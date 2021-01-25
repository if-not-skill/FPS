// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "FPS/Weapons/BaseWeapon.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	VerticalSensitivity = 1.f;
	HorizontalSensitivity = 1.f;

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
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	SpawnWeapon();
	
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("Turn", this, &APlayerCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharacter::LookUp);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);

	PlayerInputComponent->BindAction("Aiming", IE_Pressed, this, &APlayerCharacter::Aiming);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacter::StartJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &APlayerCharacter::StopJump);

}

void APlayerCharacter::SpawnWeapon()
{	
	FActorSpawnParameters SpawnParameters;
	
	CurrentWeapon = GetWorld()->SpawnActor<ABaseWeapon>(CurrentWeaponClass, FTransform(), SpawnParameters);
	CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, FName("LeftHandSocket"));
}

void APlayerCharacter::LookUp(float Axis)
{
	AddControllerPitchInput(Axis * VerticalSensitivity * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::Turn(float Axis)
{
	AddControllerYawInput(Axis * HorizontalSensitivity * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::Aiming()
{
}

void APlayerCharacter::MoveForward(float Axis)
{
	if(Axis == 0.f) return;
	
	AddMovementInput(GetActorForwardVector(), Axis);
}

void APlayerCharacter::MoveRight(float Axis)
{
	if(Axis == 0.f) return;
	
	AddMovementInput(GetActorRightVector(), Axis);
}

void APlayerCharacter::StartJump()
{
	Jump();
}

void APlayerCharacter::StopJump()
{
	StopJumping();
}

