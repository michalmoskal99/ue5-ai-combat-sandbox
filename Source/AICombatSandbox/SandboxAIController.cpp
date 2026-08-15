// Fill out your copyright notice in the Description page of Project Settings.

#include "SandboxAIController.h"
#include "AICombatSandbox.h" // LogSandboxAI
#include "UObject/ConstructorHelpers.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "BehaviorTree/BlackboardComponent.h"

ASandboxAIController::ASandboxAIController()
{
	//TYDZIEŃ 1
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BehaviorTreeObj(TEXT("/Game/AI/BT/BT_Sandbox.BT_Sandbox"));
	if (BehaviorTreeObj.Succeeded())
	{
		BehaviorTreeAsset = BehaviorTreeObj.Object;
	}
	else
	{
		UE_LOG(LogSandboxAI, Warning, TEXT("BT_Sandbox not found at expected path — sprawdz Content/AI/BT/ albo popraw sciezke."));
	}

	// --- Tydzień 2: Perception ---
	SandboxPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));

	SightConfig->SightRadius = 1000.0f;
	SightConfig->LoseSightRadius = 1200.0f;
	SightConfig->PeripheralVisionAngleDegrees = 90.0f;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	HearingConfig->HearingRange = 800.0f;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;

	SandboxPerceptionComponent->ConfigureSense(*SightConfig);
	SandboxPerceptionComponent->ConfigureSense(*HearingConfig);
}

void ASandboxAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (BehaviorTreeAsset)
	{
		RunBehaviorTree(BehaviorTreeAsset);
	}
	else
	{
		UE_LOG(LogSandboxAI, Warning, TEXT("BehaviorTreeAsset is null — nie uruchomiono BT."));
	}

	SandboxPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ASandboxAIController::OnTargetPerceptionUpdated);

	UE_LOG(LogSandboxAI, Log, TEXT("ASandboxAIController::OnPossess called for pawn: %s"), *InPawn->GetName());
}

void ASandboxAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		// TODO: GetBlackboardComponent()->SetValueAsObject(TEXT("TargetActor"), Actor);
		GetBlackboardComponent()->SetValueAsObject(TEXT("TargetActor"), Actor);
		// TODO: LastKnownLocation = Stimulus.StimulusLocation;
		LastKnownLocation = Stimulus.StimulusLocation;
		// TODO: GetWorldTimerManager().ClearTimer(ForgetTargetTimerHandle);
		GetWorldTimerManager().ClearTimer(ForgetTargetTimerHandle);
	}
	else
	{
		// TODO: GetWorldTimerManager().SetTimer(ForgetTargetTimerHandle, this, &ASandboxAIController::ForgetTarget, ForgetTargetDelay);
		GetWorldTimerManager().SetTimer(ForgetTargetTimerHandle, this, &ASandboxAIController::ForgetTarget, ForgetTargetDelay);
	}
}

void ASandboxAIController::ForgetTarget()
{
	// TODO: GetBlackboardComponent()->ClearValue(TEXT("TargetActor"));
	GetBlackboardComponent()->ClearValue(TEXT("TargetActor"));
}