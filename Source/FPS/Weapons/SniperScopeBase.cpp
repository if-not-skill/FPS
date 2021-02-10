// Fill out your copyright notice in the Description page of Project Settings.


#include "SniperScopeBase.h"

#include "Camera/CameraComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

ASniperScope::ASniperScope()
{
	SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>("SceneCapture");
	SceneCapture->SetupAttachment(GetRootComponent());
}

void ASniperScope::BeginPlay()
{
	Super::BeginPlay();

	// SetScopeMaterial();
	// SceneCapture->DestroyComponent();	
	
}
