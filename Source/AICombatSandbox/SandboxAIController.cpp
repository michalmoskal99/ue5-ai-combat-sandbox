// Fill out your copyright notice in the Description page of Project Settings.

#include "SandboxAIController.h"
#include "AICombatSandbox.h" // LogSandboxAI
#include "UObject/ConstructorHelpers.h"
#include "SandboxAIParams.h"
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

	// --- Tydzień 3: parametry AI jako data asset (tuning bez rekompilacji) ---
	static ConstructorHelpers::FObjectFinder<USandboxAIParams> AIParamsObj(TEXT("/Game/AI/Data/DA_SandboxAIParams.DA_SandboxAIParams"));
	if (AIParamsObj.Succeeded())
	{
		AIParams = AIParamsObj.Object;
	}
	else
	{
		UE_LOG(LogSandboxAI, Warning, TEXT("DA_SandboxAIParams not found at expected path — sprawdz Content/AI/Data/ albo popraw sciezke."));
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


void ASandboxAIController::AdvancePatrolPoint()
{
	// TODO: sprawdź PatrolPoints.Num() > 0 — jeśli pusta, zaloguj przez LogSandboxAI
	// (Warning, nie Error — pusty patrol to poprawny stan, np. NPC czysto stacjonarny)
	// i wyjdź z funkcji
	if (PatrolPoints.Num() == 0)
	{
		UE_LOG(LogSandboxAI, Warning, TEXT("Patrol Ponits jest rowny 0!"));
		return;
	}

	// TODO: pobierz aktora pod CurrentPatrolIndex, wywołaj GetBlackboardComponent()
	// i ustaw jego lokalizację pod kluczem PatrolPoint — zastanów się, czy SetValueAsVector
	GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPoint"), PatrolPoints[CurrentPatrolIndex]->GetActorLocation());
	
	// (lokalizacja) czy SetValueAsObject (sam actor) lepiej pasuje do tego, jak BT_Sandbox
	// już korzysta z tego klucza z Tygodnia 1

	// TODO: inkrementuj CurrentPatrolIndex, zawiń modulo do długości tablicy
	// (żeby po ostatnim punkcie wrócić do pierwszego, nie wyjść poza zakres)
	CurrentPatrolIndex = (CurrentPatrolIndex + 1) % PatrolPoints.Num();
}