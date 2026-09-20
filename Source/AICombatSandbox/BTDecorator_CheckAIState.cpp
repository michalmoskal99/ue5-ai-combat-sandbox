#include "BTDecorator_CheckAIState.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

UBTDecorator_CheckAIState::UBTDecorator_CheckAIState()
{
    NodeName = "Check AI State"; // nazwa widoczna w edytorze BT

    BlackboardKey.AddEnumFilter(this, GET_MEMBER_NAME_CHECKED(UBTDecorator_CheckAIState, BlackboardKey), StaticEnum<EAIState>());
}


bool UBTDecorator_CheckAIState::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

   
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

EBlackboardNotificationResult UBTDecorator_CheckAIState::OnBlackboardKeyValueChange(const UBlackboardComponent& Blackboard, FBlackboard::FKey ChangedKeyID)
{
    if (BlackboardKey.GetSelectedKeyID() != ChangedKeyID)
    {
        return EBlackboardNotificationResult::ContinueObserving;
    }

    UBehaviorTreeComponent* BehaviorTreeComp = Cast<UBehaviorTreeComponent>(Blackboard.GetBrainComponent());
    if (BehaviorTreeComp)
    {
        BehaviorTreeComp->RequestBranchEvaluation(*this);
    }

    return EBlackboardNotificationResult::ContinueObserving;
}