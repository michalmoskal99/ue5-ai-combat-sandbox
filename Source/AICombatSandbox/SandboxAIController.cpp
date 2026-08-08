// Fill out your copyright notice in the Description page of Project Settings.

#include "SandboxAIController.h"
#include "AICombatSandbox.h" // LogSandboxAI
#include "UObject/ConstructorHelpers.h"
#include "BehaviorTree/BehaviorTree.h"

ASandboxAIController::ASandboxAIController()
{
	// ConstructorHelpers działa WYŁĄCZNIE w konstruktorze — stąd load assetu tu,
	// nie np. w OnPossess. Hardcoded ścieżka to świadomy skrót na ten etap;
	// T3 prawdopodobnie przeniesie to do UPrimaryDataAsset (data-driven parametry AI).
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BehaviorTreeObj(TEXT("/Game/AI/BT/BT_Sandbox.BT_Sandbox"));
	if (BehaviorTreeObj.Succeeded())
	{
		BehaviorTreeAsset = BehaviorTreeObj.Object;
	}
	else
	{
		// Cichy fail-path = zero logu = godziny szukania, czemu BT nie startuje.
		UE_LOG(LogSandboxAI, Warning, TEXT("BT_Sandbox not found at expected path — sprawdz Content/AI/BT/ albo popraw sciezke."));
	}
}

void ASandboxAIController::OnPossess(APawn* InPawn)
{
	// Klasa bazowa najpierw robi swój setup (wiązanie z Pawnem) —
	// nasza logika zakłada, że to już zaszło.
	Super::OnPossess(InPawn);

	if (BehaviorTreeAsset)
	{
		// RunBehaviorTree startuje drzewo i PRZY OKAZJI inicjalizuje sparowany
		// z nim Blackboard (BB_Sandbox) — nie trzeba robić tego osobno.
		RunBehaviorTree(BehaviorTreeAsset);
	}
	else
	{
		UE_LOG(LogSandboxAI, Warning, TEXT("BehaviorTreeAsset is null — nie uruchomiono BT."));
	}

	// Dowód, że possession zaszedł i wskazuje właściwy Pawn/Controller.
	// %s wymaga surowego wskaźnika na tekst — gwiazdka wyciąga go z FString.
	UE_LOG(LogSandboxAI, Log, TEXT("ASandboxAIController::OnPossess called for pawn: %s"), *InPawn->GetName());
}