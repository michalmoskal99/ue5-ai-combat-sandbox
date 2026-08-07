// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SandboxAIController.generated.h"

// "Mózg" NPC — possessuje SandboxAICharacter; w kolejnych tygodniach
// dojdzie tu percepcja (AIPerceptionComponent) i start Behavior Tree.

UCLASS()
class AICOMBATSANDBOX_API ASandboxAIController : public AAIController
{
	GENERATED_BODY()

protected:

	// Odpala się automatycznie przy przejęciu Pawna —
	// na razie tylko log, docelowo tu wyląduje start percepcji i BT.
	virtual void OnPossess(APawn* InPawn) override;
	
};
