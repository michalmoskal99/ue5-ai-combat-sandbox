#include "EnvQueryTest_CoverAngleToThreat.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_VectorBase.h"
#include "SandboxAIController.h"

UEnvQueryTest_CoverAngleToThreat::UEnvQueryTest_CoverAngleToThreat()
{
	// TODO: Cost = EEnvTestCost::High — trace per item, znacznie droższe niż DistanceToThreat
	// TODO: ValidItemType = UEnvQueryItemType_VectorBase::StaticClass() — jak w poprzednim teście
	// TODO: SetWorkOnBoolValues(true) — wynik to blocked/not blocked, nie skala
	Cost = EEnvTestCost::High;
	ValidItemType = UEnvQueryItemType_VectorBase::StaticClass();
	SetWorkOnFloatValues(false);
}

void UEnvQueryTest_CoverAngleToThreat::RunTest(FEnvQueryInstance& QueryInstance) const
{
	// TODO: ten sam wzorzec Owner->GetInstigatorController()->Cast co w poprzednim teście
	//       (masz to już napisane raz — to jest dokładnie ten moment, gdzie się przydaje
	//       nie kopiować ślepo, tylko rozpoznać powtarzający się wzorzec)
	// TODO: ThreatLocation z SandboxAIController->GetThreatLocation() — jak poprzednio
	AActor* QueryOwner = Cast<AActor>(QueryInstance.Owner.Get());
	ASandboxAIController* MyController = QueryOwner
		? Cast<ASandboxAIController>(QueryOwner->GetInstigatorController()) 
		: nullptr;

	// TODO: FCollisionQueryParams — zbuduj i AddIgnoredActor() na aktorze zagrożenia,
	//       inaczej trace zawsze "trafi" w cel na końcu linii (patrz wyjaśnienie wyżej)

	FCollisionQueryParams TraceParams;

	

	// TODO: pętla ItemIterator, jak w poprzednim teście
	// TODO: w pętli — GetItemLocation(QueryInstance, It.GetIndex()) jak poprzednio
	// TODO: GetWorld()->LineTraceSingleByChannel(HitResult, ItemLocation, ThreatLocation,
	//       ECC_Visibility, TwojeQueryParams) — zwraca bool: true = coś zablokowało
	// TODO: It.SetScore(TestPurpose, FilterType, WynikBoolean, /* bWantsHit */ true)
	//       — sprawdź w EnvQueryTest.h dokładną sygnaturę przeciążenia dla bool,
	//       różni się parametrami od wersji float, której już używałeś
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