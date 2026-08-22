#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "SandboxAITypes.h"
#include "BTDecorator_CheckAIState.generated.h"

UCLASS()
class AICOMBATSANDBOX_API UBTDecorator_CheckAIState : public UBTDecorator
{
    GENERATED_BODY()
public:
    UBTDecorator_CheckAIState();

protected:
    UPROPERTY(EditAnywhere, Category = "AI")
    EAIState RequiredState = EAIState::Idle;

    virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};