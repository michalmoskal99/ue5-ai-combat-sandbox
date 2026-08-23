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
#include "Perception/AISense.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"

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

	SightConfig->SightRadius = 0.0f;//DO ZMIANY PAMIETAC!
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
		if(Stimulus.Type == UAISense::GetSenseID(UAISense_Sight::StaticClass()))
		{ 
			// TODO: GetBlackboardComponent()->SetValueAsObject(TEXT("TargetActor"), Actor);
			GetBlackboardComponent()->SetValueAsObject(TEXT("TargetActor"), Actor);
			// TODO: LastKnownLocation = Stimulus.StimulusLocation;
			LastKnownLocation = Stimulus.StimulusLocation;
			// TODO: GetWorldTimerManager().ClearTimer(ForgetTargetTimerHandle);
			GetWorldTimerManager().ClearTimer(ForgetTargetTimerHandle);


			// TODO: SetValueAsEnum na Combat — Sight ma najwyższy priorytet,
			// więc bez żadnego warunku (w przeciwieństwie do Hearing, tu nie
			// sprawdzamy aktualnego stanu — wzrok zawsze wygrywa)
			GetBlackboardComponent()->SetValueAsEnum(TEXT("AIState"), static_cast<uint8>(EAIState::Combat));

			// TODO: GetWorldTimerManager().ClearTimer(SuspiciousToAlertTimerHandle)
			// — jeśli timer z kroku 2 już leci, a NPC właśnie zobaczył gracza
			// bezpośrednio, timer eskalujący do Alert jest już bez znaczenia
			// (Combat jest wyżej), zostawienie go włączonego to nieszkodliwy,
			// ale zbędny odpad
			GetWorldTimerManager().ClearTimer(SuspiciousToAlertTimerHandle);
		}
		else if (Stimulus.Type == UAISense::GetSenseID(UAISense_Hearing::StaticClass()))
		{
			// TODO: odczytaj aktualny AIState (GetValueAsEnum, ten sam wzorzec co w Decoratorze)
		   // TODO: jeśli aktualny stan to Idle LUB Patrol (porównanie z static_cast<uint8>(EAIState::...))
		   //       — dopiero wtedy SetValueAsEnum na Suspicious
		   // (celowo NIE dotykamy Suspicious/Alert/Search/Combat — hearing nie może
		   // cofnąć NPC z czegoś pilniejszego)
			EAIState CurrentState = static_cast<EAIState>(GetBlackboardComponent()->GetValueAsEnum(TEXT("AIState")));
			if (CurrentState == EAIState::Idle || CurrentState == EAIState::Patrol)
			{
				GetBlackboardComponent()->SetValueAsEnum(TEXT("AIState"), static_cast<uint8>(EAIState::Suspicious));
				GetWorldTimerManager().SetTimer(SuspiciousToAlertTimerHandle, this, &ASandboxAIController::EscalateToAlert, AIParams->SuspiciousToAlertDelay);
			}
		}
		
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

void ASandboxAIController::EscalateToAlert()
{
	// TODO: SetValueAsEnum na Alert — bez warunku, bo ten timer
	// z definicji odpala się tylko wtedy, gdy stan wciąż jest Suspicious
	// (patrz punkt 3 — start timera dzieje się tylko przy wejściu w Suspicious)
	GetBlackboardComponent()->SetValueAsEnum(TEXT("AIState"), static_cast<uint8>(EAIState::Alert));
}