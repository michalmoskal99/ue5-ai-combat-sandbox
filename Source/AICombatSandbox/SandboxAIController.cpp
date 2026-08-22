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
#include "SandboxAICharacter.h"

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
		// TODO: tutaj — zaraz po RunBehaviorTree, bo dopiero teraz Blackboard
		// na pewno istnieje. GetBlackboardComponent()->SetValueAsEnum(klucz, wartość),
		// wartość rzutowana na uint8 tak jak RequiredState w Decoratorze
		GetBlackboardComponent()->SetValueAsEnum(TEXT("AIState"), static_cast<uint8>(EAIState::Patrol));

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
	// Kontroler nie ma bezpośredniego dostępu do PatrolPoints — te dane żyją na postaci
	// (bo to ona fizycznie stoi w TestArena, nie kontroler, który jest tworzony dynamicznie
	// przy possession). GetPawn() zwraca ogólny APawn*, więc trzeba go zawęzić Castem
	// do konkretnego typu, na którym zadeklarowaliśmy PatrolPoints.
	ASandboxAICharacter* SandboxAICharacter = Cast<ASandboxAICharacter>(GetPawn());
	if (!SandboxAICharacter)
	{
		// Cast<> może się nie udać (np. inny typ postaci podpięty przez pomyłkę)
		// — zabezpieczenie przed wywołaniem dalszego kodu na nullptrze (crash).
		UE_LOG(LogSandboxAI, Warning, TEXT("AdvancePatrolPoint: GetPawn() nie jest ASandboxAICharacter!"));
		return;
	}

	// Pusty patrol to poprawny stan (np. NPC czysto stacjonarny), nie błąd —
	// stąd Warning, nie Error, i zwykły wczesny return zamiast dalszego indeksowania.
	if (SandboxAICharacter->PatrolPoints.Num() == 0)
	{
		UE_LOG(LogSandboxAI, Warning, TEXT("PatrolPoints jest puste!"));
		return;
	}

	// Zapisujemy samą lokalizację (Vector), nie referencję do aktora (Object) —
	// bo klucz PatrolPoint w BB_Sandbox jest zadeklarowany jako typ Vector.
	// GetActorLocation() wyciąga pozycję z transformu wybranego punktu patrolu.
	GetBlackboardComponent()->SetValueAsVector(
		TEXT("PatrolPoint"),
		SandboxAICharacter->PatrolPoints[CurrentPatrolIndex]->GetActorLocation()
	);

	// Modulo zawija indeks do długości tablicy — po ostatnim punkcie NPC wraca
	// do pierwszego, zamiast wyjść poza zakres (co byłoby odczytem pamięci
	// poza granicami tablicy, klasyczny crash).
	CurrentPatrolIndex = (CurrentPatrolIndex + 1) % SandboxAICharacter->PatrolPoints.Num();
}