// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SandboxAICharacter.generated.h"


// Bazowa klasa NPC w sandboxie. ACharacter, nie goły APawn —
// od razu dostajemy kolizję (Capsule) i ruch po navmeshu (CharacterMovementComponent).
UCLASS()

class AICOMBATSANDBOX_API ASandboxAICharacter : public ACharacter
{
	GENERATED_BODY()

public:

	// --- Tydzień 3: patrol ---
	UPROPERTY(EditAnywhere, Category = "AI|Patrol")
	TArray<AActor*> PatrolPoints;

	// Sets default values for this character's properties
	// Konstruktor — tu ustawiamy domyślne wartości (np. klasę AI Controllera),
	// zanim jeszcze cokolwiek wydarzy się w grze.
	ASandboxAICharacter();

protected:
	// Called when the game starts or when spawned
	// Wywoływane, gdy gra się zaczyna (lub actor zostaje zespawnowany) —
	// na razie puste, przyszła logika (np. start percepcji) wyląduje tu w kolejnych tygodniach.
	virtual void BeginPlay() override;



};
