// Fill out your copyright notice in the Description page of Project Settings.


#include "SandboxAICharacter.h"
#include "SandboxAIController.h"

// Sets default values
ASandboxAICharacter::ASandboxAICharacter()
{
 	
	// Klasa kontrolera, która przejmie tego Pawna przy auto-possess
	AIControllerClass = ASandboxAIController::StaticClass();

	// PlacedInWorldOrSpawned, nie samo PlacedInWorld — za parę tygodni część NPC
	// będzie spawnowana w runtime (squad), nie tylko stawiana ręcznie na mapie
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

}

// Called when the game starts or when spawned
void ASandboxAICharacter::BeginPlay()
{
	Super::BeginPlay();
	// Na razie pusto — przyszła logika (np. start percepcji) wyląduje tu w kolejnych tygodniach.
	
}
