// EnvQueryContext_Player.h
#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "EnvQueryContext_Player.generated.h"

// Zwraca aktualne zagrożenie (gracza) jako Context dla EQS_FindAttackPosition —
// dziedziczy po UEnvQueryContext, nie po EnvQueryContext_Querier, bo Querier
// zwraca NPC, a tu potrzebny jest odwrotny punkt odniesienia.
UCLASS()
class AICOMBATSANDBOX_API UEnvQueryContext_Player : public UEnvQueryContext
{
	GENERATED_BODY()

public:
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;
};