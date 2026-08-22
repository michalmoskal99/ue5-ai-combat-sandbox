
#include "BTTask_AdvancePatrolPoint.h"
#include "SandboxAIController.h"

UBTTask_AdvancePatrolPoint::UBTTask_AdvancePatrolPoint()
{
    NodeName = "Advance Patrol Point";
}

EBTNodeResult::Type UBTTask_AdvancePatrolPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // TODO: wyciągnij AAIController* z OwnerComp (metoda GetAIOwner()),
    // a potem rzutuj go (Cast<...>) na ASandboxAIController
    ASandboxAIController* SandboxAIController = Cast<ASandboxAIController>(OwnerComp.GetAIOwner());

    // TODO: zabezpieczenie — jeśli rzutowanie się nie uda (nullptr),
    // zwróć EBTNodeResult::Failed
    if (!SandboxAIController)
    {
        return EBTNodeResult::Failed;
    }

    // TODO: wywołaj AdvancePatrolPoint() na otrzymanym wskaźniku,
    // zwróć EBTNodeResult::Succeeded — ten Task ma się wykonać
    // i zakończyć w tej samej klatce, nie czekać (InProgress nie jest tu potrzebne)
	SandboxAIController->AdvancePatrolPoint();
	return EBTNodeResult::Succeeded;
}