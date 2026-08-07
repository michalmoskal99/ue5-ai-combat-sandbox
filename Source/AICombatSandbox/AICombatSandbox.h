// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"


// Własna kategoria logów AI (zamiast domyślnego LogAICombatSandbox) —
// pozwala filtrować w Output Logu wyłącznie komunikaty z systemów AI.
// Deklaracja tu (widoczna wszędzie po include), definicja w .cpp.
DECLARE_LOG_CATEGORY_EXTERN(LogSandboxAI, Log, All);