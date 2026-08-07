// Copyright Epic Games, Inc. All Rights Reserved.

#include "AICombatSandbox.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, AICombatSandbox, "AICombatSandbox" );

// Definicja kategorii zadeklarowanej w .h — bez tej linii błąd linkera
// (deklaracja obiecuje, że symbol istnieje; tu faktycznie istnieje).
DEFINE_LOG_CATEGORY(LogSandboxAI);