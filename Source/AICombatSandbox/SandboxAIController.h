// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SandboxAIController.generated.h"

class UBehaviorTree;

// "Mózg" NPC — possessuje SandboxAICharacter i uruchamia jego Behavior Tree.
// W kolejnych tygodniach dojdzie tu percepcja (AIPerceptionComponent).
UCLASS()
class AICOMBATSANDBOX_API ASandboxAIController : public AAIController
{
	GENERATED_BODY()

public:
	ASandboxAIController();

protected:
	// Wskazany w konstruktorze (ConstructorHelpers) na BT_Sandbox — na razie
	// hardcoded reference, T3 prawdopodobnie przeniesie to do UPrimaryDataAsset.
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;

	// Odpala się automatycznie przy przejęciu Pawna — startuje Behavior Tree
	// i loguje possession. W kolejnych tygodniach dojdzie tu start percepcji.
	virtual void OnPossess(APawn* InPawn) override;
};