// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ScopeBase.h"
#include "SniperScopeBase.generated.h"

/**
 * 
 */
UCLASS()
class FPS_API ASniperScope : public AScopeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	USceneCaptureComponent2D* SceneCapture;

public:
	ASniperScope();

	UFUNCTION(BlueprintImplementableEvent)
	void SetScopeMaterial();

	virtual void BeginPlay() override;
	
};
