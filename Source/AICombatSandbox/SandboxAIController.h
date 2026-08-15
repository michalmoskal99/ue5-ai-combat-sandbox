// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "TimerManager.h"
#include "SandboxAIController.generated.h"

class UBehaviorTree;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;

// "Mózg" NPC — possessuje SandboxAICharacter i uruchamia jego Behavior Tree.
UCLASS()
class AICOMBATSANDBOX_API ASandboxAIController : public AAIController
{
	GENERATED_BODY()

public:
	ASandboxAIController();

protected:

	//TYDZIEŃ 1
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;

	virtual void OnPossess(APawn* InPawn) override;

	//TYDZIEŃ 2
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	// UWAGA: AAIController juz ma wlasne pole "PerceptionComponent" (uzywane wewnetrznie
	// przez silnik, dostepne przez GetAIPerceptionComponent()). Stad inna nazwa ponizej —
	// wlasna deklaracja "PerceptionComponent" = "shadowing", ktorego UHT nie pozwala zrobic.
	UPROPERTY(VisibleAnywhere, Category = "AI|Perception")
	TObjectPtr<UAIPerceptionComponent> SandboxPerceptionComponent;

	UPROPERTY()
	TObjectPtr<UAISenseConfig_Sight> SightConfig;

	UPROPERTY()
	TObjectPtr<UAISenseConfig_Hearing> HearingConfig;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Perception")
	float ForgetTargetDelay = 5.0f;

	FVector LastKnownLocation = FVector::ZeroVector;
	FTimerHandle ForgetTargetTimerHandle;

	void ForgetTarget();
};