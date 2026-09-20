#include "EnvQueryTest_CoverAngleToThreat.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_VectorBase.h"
#include "SandboxAIController.h"

UEnvQueryTest_CoverAngleToThreat::UEnvQueryTest_CoverAngleToThreat()
{
	
	Cost = EEnvTestCost::High;
	ValidItemType = UEnvQueryItemType_VectorBase::StaticClass();
	SetWorkOnFloatValues(false);
}

void UEnvQueryTest_CoverAngleToThreat::RunTest(FEnvQueryInstance& QueryInstance) const
{
	
	AActor* QueryOwner = Cast<AActor>(QueryInstance.Owner.Get());
	ASandboxAIController* MyController = QueryOwner
		? Cast<ASandboxAIController>(QueryOwner->GetInstigatorController()) 
		: nullptr;



	FCollisionQueryParams TraceParams;

	


	FHitResult HitResult;
	if (!MyController)
	{
		return;
	}
	else
	{ 
		AActor* ThreatActor = MyController->GetThreatActor();
		if (ThreatActor)
		{
			TraceParams.AddIgnoredActor(ThreatActor);
		}
		for (FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It)
		{
			FVector ItemLocation = GetItemLocation(QueryInstance, It.GetIndex());
			bool bBlocked = MyController->GetWorld()->LineTraceSingleByChannel(
				HitResult,
				ItemLocation,
				MyController->GetThreatLocation(),
				ECC_Visibility,
				TraceParams
			);
			It.SetScore(TestPurpose, FilterType, bBlocked, true);

		}
	}
	
	
}

FText UEnvQueryTest_CoverAngleToThreat::GetDescriptionTitle() const
{
	return FText::FromString(TEXT("Cover Angle To Threat"));
}

FText UEnvQueryTest_CoverAngleToThreat::GetDescriptionDetails() const
{
	return FText::FromString(TEXT("Checks whether line of sight to the current threat is blocked (trace-based occlusion)."));
}