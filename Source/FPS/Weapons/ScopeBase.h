// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ScopeBase.generated.h"


UCLASS()
class FPS_API AScopeBase : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UStaticMeshComponent* ScopeMesh;
	
public:
	AScopeBase();

protected:
	virtual void BeginPlay() override;
};
