// EnvQueryTest_DistanceToThreat.cpp
#include "EnvQueryTest_DistanceToThreat.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_VectorBase.h"
// TODO: include na nagłówek Twojego AIControllera (Cast poniżej)
#include "SandboxAIController.h"
#include "AICombatSandbox.h"

UEnvQueryTest_DistanceToThreat::UEnvQueryTest_DistanceToThreat()
{
	Cost = EEnvTestCost::Low;
	ValidItemType = UEnvQueryItemType_VectorBase::StaticClass();
	SetWorkOnFloatValues(true);
}

void UEnvQueryTest_DistanceToThreat::RunTest(FEnvQueryInstance& QueryInstance) const
{
	// QueryInstance.Owner to PAWN wykonujący zapytanie, nie jego kontroler —
	// stąd dwustopniowe dojście: Owner -> GetInstigatorController() -> Cast.
	// Bezpośredni Cast<ASandboxAIController>(Owner) kompiluje się bez ostrzeżenia,
	// ale zawsze zwraca nullptr (pawn i kontroler to rozłączne gałęzie hierarchii).
	AActor* QueryOwner = Cast<AActor>(QueryInstance.Owner.Get());
	ASandboxAIController* SandboxAIController = QueryOwner
		? Cast<ASandboxAIController>(QueryOwner->GetInstigatorController())
		: nullptr;
	
	if (SandboxAIController)
	{
		FloatValueMin.BindData(SandboxAIController, QueryInstance.QueryID);
		const float MinThreshold = FloatValueMin.GetValue();

		FloatValueMax.BindData(SandboxAIController, QueryInstance.QueryID);
		const float MaxThreshold = FloatValueMax.GetValue();

		const FVector ThreatLocation = SandboxAIController->GetThreatLocation();

		for (FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It)
		{
			// TODO: ItemLocation = GetItemLocation(QueryInstance, It.GetIndex())
			// TODO: Score = FVector::Dist(ItemLocation, ThreatLocation)
			// TODO: It.SetScore(TestPurpose, FilterType, Score, FilterMin, FilterMax)
			FVector ItemLocation = GetItemLocation(QueryInstance, It.GetIndex());
			float Score = FVector::Dist(ItemLocation, ThreatLocation);
			It.SetScore(TestPurpose, FilterType, Score, MinThreshold, MaxThreshold);
		}
	}
	else
	{
		UE_LOG(LogSandboxAI, Warning, TEXT("Owner query nie jest Twoim kontrolerem"));
		
	}

	

	


}

// TODO: GetDescriptionTitle/Details — krótki tekst, który zobaczysz w edytorze EQS przy tym teście
FText UEnvQueryTest_DistanceToThreat::GetDescriptionTitle() const
{
	return FText::FromString(TEXT("Distance To Threat"));
}

FText UEnvQueryTest_DistanceToThreat::GetDescriptionDetails() const
{
	return FText::FromString(TEXT("Filters/scores points by distance to the current threat location (Combat: TargetActor, Search: LastKnownLocation)."));
}