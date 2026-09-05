// EnvQueryTest_DistanceToThreat.h
#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTest.h"
#include "EnvQueryTest_DistanceToThreat.generated.h"

/**
 * Filtr: odrzuca punkty zbyt blisko zagrożenia — kryjówka nie może
 * być tam, gdzie i tak nie ma sensu się chować.
 */
UCLASS()
class AICOMBATSANDBOX_API UEnvQueryTest_DistanceToThreat : public UEnvQueryTest
{
	GENERATED_BODY()

public:
	UEnvQueryTest_DistanceToThreat();

	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;
	virtual FText GetDescriptionTitle() const override;
	virtual FText GetDescriptionDetails() const override;
};