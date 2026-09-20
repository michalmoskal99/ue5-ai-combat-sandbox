#pragma once

#include "CoreMinimal.h"
#include "SandboxAITypes.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "BTDecorator_CheckAIState.generated.h"

UCLASS()
class AICOMBATSANDBOX_API UBTDecorator_CheckAIState : public UBTDecorator_BlackboardBase
{
    GENERATED_BODY()
public:
    UBTDecorator_CheckAIState();

protected:
    UPROPERTY(EditAnywhere, Category = "AI")
    EAIState RequiredState = EAIState::Idle;

    virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

    virtual EBlackboardNotificationResult OnBlackboardKeyValueChange(const UBlackboardComponent& Blackboard, FBlackboard::FKey ChangedKeyID) override;
};