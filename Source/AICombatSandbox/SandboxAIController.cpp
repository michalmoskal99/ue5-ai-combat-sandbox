// Fill out your copyright notice in the Description page of Project Settings.


#include "SandboxAIController.h"
#include "AICombatSandbox.h"

void ASandboxAIController::OnPossess(APawn* InPawn)
{
	// Klasa bazowa najpierw robi swój setup (wiązanie z Pawnem) —
	// nasza logika zakłada, że to już zaszło.
	Super::OnPossess(InPawn);


	// Dowód, że possession zaszedł i wskazuje właściwy Pawn/Controller.
	// %s wymaga surowego wskaźnika na tekst — gwiazdka wyciąga go z FString.
	UE_LOG(LogSandboxAI, Log, TEXT("ASandboxAIController::OnPossess called for pawn: %s"), *InPawn->GetName());
}

