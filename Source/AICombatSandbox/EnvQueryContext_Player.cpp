// EnvQueryContext_Player.cpp
#include "EnvQueryContext_Player.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "SandboxAIController.h"
#include "AICombatSandbox.h"

void UEnvQueryContext_Player::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	// Owner w EQS to zawsze Pawn, nie Controller — bezpośredni Cast<AController>
	// zwróciłby nullptr bez ostrzeżenia kompilatora (lekcja z T5, CoverAngleToThreat).
	AActor* OwnerActor = Cast<AActor>(QueryInstance.Owner.Get());
	if (!OwnerActor)
	{
		return;
	}

	ASandboxAIController* SandboxAIController = Cast<ASandboxAIController>(OwnerActor->GetInstigatorController());

	// Wołamy GetThreatActor() z kontrolera zamiast samemu czytać AIState z Blackboarda —
	// nie chodzi o brak dostępu (mamy ten sam kontroler), tylko o to, że switch po AIState
	// ma jedno źródło prawdy na kontrolerze. Trzecie miejsce w projekcie stosujące tę zasadę,
	// po GetThreatLocation() i AddIgnoredActor() w CoverAngleToThreat (T5).
	AActor* ThreatActor = SandboxAIController ? SandboxAIController->GetThreatActor() : nullptr;

	if (!ThreatActor)
	{
		// Puste w stanach innych niż Combat (np. Search/default) — Context bez danych
		// i tak nie da EQS niczego sensownego do wygenerowania, stąd early return + log
		// zamiast cichego przejścia dalej.
		UE_LOG(LogSandboxAI, Warning, TEXT("EnvQueryContext_Player: brak ThreatActor (GetThreatActor zwrócił nullptr)."));
		return;
	}

	UEnvQueryItemType_Actor::SetContextHelper(ContextData, ThreatActor);
}