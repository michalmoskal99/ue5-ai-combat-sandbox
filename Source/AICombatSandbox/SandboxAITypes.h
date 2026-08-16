#pragma once

#include "CoreMinimal.h"
#include "SandboxAITypes.generated.h" // musi być ostatni include w pliku

// Wspólny enum stanu AI używany w Blackboardzie (klucz AIState) i w Decoratorze CheckAIState —
// jeden typ, żeby kontroler, Decorator i przyszłe Service'y nie rozjechały się w interpretacji stanu
UENUM(BlueprintType)
enum class EAIState : uint8
{
    Idle,
    Patrol,
    Suspicious,
    Alert,
    Search,
    Combat,
};