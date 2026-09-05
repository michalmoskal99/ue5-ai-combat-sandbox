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

namespace SandboxAIDebugCVars
{
	static TAutoConsoleVariable<int32> CVarAIDebug(
		TEXT("sandbox.ai.debug"),
		0,
		TEXT("AI Combat Sandbox debug overlay (FOV cone + stan NPC). 0 = off, 1 = on."),
		ECVF_Cheat);
}

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
	SightConfig->PeripheralVisionAngleDegrees = 45.0f;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	HearingConfig->HearingRange = 800.0f;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;

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

	FString SenseName = TEXT("Unknown");
	if (Stimulus.Type == UAISense::GetSenseID(UAISense_Sight::StaticClass()))
	{
		SenseName = TEXT("Sight");
	}
	else if (Stimulus.Type == UAISense::GetSenseID(UAISense_Hearing::StaticClass()))
	{
		SenseName = TEXT("Hearing");
	}

	UE_LOG(LogTemp, Warning, TEXT("Perception updated: Actor=%s, Sense=%s, Successful=%d"),
		*Actor->GetName(), *SenseName, Stimulus.WasSuccessfullySensed());


	if (Stimulus.WasSuccessfullySensed())
	{
		if (Stimulus.Type == UAISense::GetSenseID(UAISense_Sight::StaticClass()))
		{
			// Sight ma najwyższy priorytet — nadpisuje AIState bezwarunkowo, z dowolnego
			// stanu (w przeciwieństwie do Hearing, patrz gałąź niżej). Powód w Biblii:
			// asymetria eskalacji wg pewności sygnału.
			GetBlackboardComponent()->SetValueAsObject(TEXT("TargetActor"), Actor);
			LastKnownLocation = Stimulus.StimulusLocation;

			// Kasujemy oba timery — jeśli NPC właśnie zobaczył gracza wprost, ani
			// odliczanie do zapomnienia celu (Search), ani eskalacja Suspicious->Alert
			// nie mają już znaczenia. Ten ClearTimer jest też tym, co pozwala Search
			// wrócić do Combat bez żadnej dodatkowej logiki — patrz Biblia.
			GetWorldTimerManager().ClearTimer(ForgetTargetTimerHandle);
			GetWorldTimerManager().ClearTimer(SuspiciousToAlertTimerHandle);

			GetBlackboardComponent()->SetValueAsEnum(TEXT("AIState"), static_cast<uint8>(EAIState::Combat));
		}
		else if (Stimulus.Type == UAISense::GetSenseID(UAISense_Hearing::StaticClass()))
		{
			// Hearing eskaluje tylko z Idle/Patrol — słabszy sygnał (przesłanka, nie
			// potwierdzenie) nie może cofnąć NPC z czegoś pilniejszego (Suspicious,
			// Alert, Search, Combat). Zasada opisana w Biblii.
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
		if (Stimulus.Type == UAISense::GetSenseID(UAISense_Sight::StaticClass()))
		{
			// Tylko utrata Sight uruchamia Search — Hearing nigdy nie ustawiało Combat,
			// więc jego wygaśnięcie (po MaxAge configu) nie wymaga żadnej reakcji stanu.
			GetBlackboardComponent()->SetValueAsEnum(TEXT("AIState"), static_cast<uint8>(EAIState::Search));

			// Blackboard, nie tylko pole C++ — BTTask_MoveTo w gałęzi Search czyta
			// LastKnownLocation stamtąd, samo pole na kontrolerze mu nie wystarczy.
			GetBlackboardComponent()->SetValueAsVector(TEXT("LastKnownLocation"), Stimulus.StimulusLocation);

			// ForgetTargetDelay pełni tu podwójną rolę: to zarówno grace period sprzed T2
			// (na wypadek chwilowego okluzji-mrugnięcia), jak i teraz czas trwania Search —
			// jeśli NPC znów zobaczy gracza, gałąź wyżej kasuje ten timer (patrz Biblia).
			GetWorldTimerManager().SetTimer(ForgetTargetTimerHandle, this, &ASandboxAIController::ForgetTarget, ForgetTargetDelay);
		}
	}
}

void ASandboxAIController::ForgetTarget()
{
	// Ścieżka rezygnacji: ForgetTargetDelay minął bez ponownego zobaczenia gracza —
	// Search kończy się porażką, wracamy do Patrol i czyścimy cel.
	GetBlackboardComponent()->ClearValue(TEXT("TargetActor"));
	GetBlackboardComponent()->SetValueAsEnum(TEXT("AIState"), static_cast<uint8>(EAIState::Patrol));
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

// --- Tydzień 4: debug overlay ---
void ASandboxAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	DrawDebugOverlay();
		
}
void ASandboxAIController::DrawDebugOverlay() const
{
	if (SandboxAIDebugCVars::CVarAIDebug.GetValueOnGameThread() == 0)
	{
		return;
	}

	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn)
	{
		return;
	}

	// Stożek FOV rysowany z SightConfig (T2), nie na sztywno — zmiana tuningu w configu
	// od razu odzwierciedla się w overlayu. AngleWidth/AngleHeight w DrawDebugCone są
	// radianowe i już połówkowe (mierzone od osi), tak samo jak PeripheralVisionAngleDegrees —
	// stąd tylko konwersja stopnie->radiany, bez dodatkowego /2 czy *2.
	// Pułapka z tej sesji: PeripheralVisionAngleDegrees miał domyślną wartość silnika (90°),
	// nieustawioną jawnie w T2 — realnie dawało to ~180° FOV. Overlay ujawnił to wizualnie,
	// poprawione na 45°.
	DrawDebugCone(
		GetWorld(),
		ControlledPawn->GetActorLocation(),
		ControlledPawn->GetActorForwardVector(),
		SightConfig->SightRadius,
		FMath::DegreesToRadians(SightConfig->PeripheralVisionAngleDegrees),
		FMath::DegreesToRadians(SightConfig->PeripheralVisionAngleDegrees),
		12, // liczba segmentów (im więcej, tym gładziej)
		FColor::Green,
		false, // nie trwały
		-1.0f, // czas życia (nie trwały więc nie ma znaczenia)
		0, // depth priority (0 = normalny)
		1.0f // grubość linii
	);

	// AIState czytany z Blackboarda — ten sam klucz, którego używa Decorator CheckAIState,
	// więc to jedyne źródło prawdy, nie duplikujemy stanu lokalnie na kontrolerze.
	// StaticEnum<EAIState>() zwraca obiekt UEnum wygenerowany przez UHT (wymaga
	// UENUM(BlueprintType) w deklaracji) — GetNameStringByValue daje samą nazwę bez
	// prefiksu klasy. UEnum::GetValueAsString<T> odpadło — w tym środowisku wywalało
	// kompilację (TEnumAsByte, static_assert w EnumAsByte.h), patrz Raport inżyniera.
	const EAIState CurrentState = static_cast<EAIState>(GetBlackboardComponent()->GetValueAsEnum(TEXT("AIState")));
	const FString State = StaticEnum<EAIState>()->GetNameStringByValue(static_cast<int64>(CurrentState));

	DrawDebugString(
		GetWorld(),
		ControlledPawn->GetActorLocation() + FVector(0, 0, 100), // nad głową
		State,
		nullptr,
		FColor::White,
		0.0f, // czas życia (0 = tylko jeden frame, rysujemy co Tick)
		true // cień pod tekstem — czytelność na jasnym tle
	);

	// SandboxAIController.cpp
	
}
FVector ASandboxAIController::GetThreatLocation() const
{
	const EAIState CurrentState = static_cast<EAIState>(GetBlackboardComponent()->GetValueAsEnum(TEXT("AIState")));

	switch (CurrentState)
	{
	case EAIState::Combat:
	{
		AActor* TargetActor = Cast<AActor>(GetBlackboardComponent()->GetValueAsObject(TEXT("TargetActor")));
		if (TargetActor != nullptr)
		{
			return TargetActor->GetActorLocation();
		}
		else
		{
			UE_LOG(LogSandboxAI, Warning, TEXT("GetThreatLocation wywołane w stanie Combat, ale TargetActor jest nullptr!"));
			return FVector::ZeroVector;
		}
	}

	case EAIState::Search:
		return LastKnownLocation;

	default:
		UE_LOG(LogSandboxAI, Warning, TEXT("GetThreatLocation wywołane w stanie bez zagrożenia (AIState=%s)"),
			*StaticEnum<EAIState>()->GetNameStringByValue(static_cast<int64>(CurrentState)));
		return FVector::ZeroVector;
	}
}