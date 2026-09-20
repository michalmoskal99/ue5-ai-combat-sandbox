// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SandboxAIParams.generated.h"

// Parametry AI jako dane, nie stałe w kodzie — zmiana zasięgu czy czasu
// nie wymaga rekompilacji, tylko edycji instancji tego assetu w edytorze
UCLASS()
class AICOMBATSANDBOX_API USandboxAIParams : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditDefaultsOnly, Category = "Suspicious")
    float SuspiciousToAlertDelay = 3.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Search")
    float SearchDuration = 8.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Patrol")
    float PatrolPointTolerance = 50.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float PatrolSpeed = 300.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float CombatSpeed = 600.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Squad")
    float CallForHelpRadius = 1500.0f;
};


