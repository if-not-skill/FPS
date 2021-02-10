// Fill out your copyright notice in the Description page of Project Settings.


#include "ScopeBase.h"

AScopeBase::AScopeBase()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	ScopeMesh = CreateDefaultSubobject<UStaticMeshComponent>("ScopeMesh");
	ScopeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetRootComponent(ScopeMesh);
}

void AScopeBase::BeginPlay()
{
	Super::BeginPlay();
	
}