// BTTask_AdvancePatrolPoint.h
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_AdvancePatrolPoint.generated.h"

UCLASS()
class AICOMBATSANDBOX_API UBTTask_AdvancePatrolPoint : public UBTTaskNode
{
    GENERATED_BODY()
public:
    UBTTask_AdvancePatrolPoint();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};