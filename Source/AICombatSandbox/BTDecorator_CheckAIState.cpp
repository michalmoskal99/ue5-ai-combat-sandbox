#include "BTDecorator_CheckAIState.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

UBTDecorator_CheckAIState::UBTDecorator_CheckAIState()
{
    NodeName = "Check AI State"; // nazwa widoczna w edytorze BT
}

bool UBTDecorator_CheckAIState::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    // TODO: wyciągnij UBlackboardComponent* z OwnerComp (metoda zwracająca
    // komponent Blackboarda z przekazanego UBehaviorTreeComponent)
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

    // TODO: jeśli wskaźnik jest null, zwróć false (zabezpieczenie, na wzór
    // guardów które już stosujesz w AdvancePatrolPoint)
    if (BlackboardComp == nullptr)
    {
        return false;
    }

    // TODO: odczytaj wartość klucza "AIState" jako enum (jedna z metod
    // GetValueAsX z rodziny Blackboarda — szukaj wariantu dla Enum, zwraca uint8)
    // i porównaj z (uint8)RequiredState, zwróć wynik porównania
	uint8 CurrentState = BlackboardComp->GetValueAsEnum("AIState");
    return CurrentState == (uint8)RequiredState;
}