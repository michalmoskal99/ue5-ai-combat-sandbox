#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTest.h"
#include "EnvQueryTest_CoverAngleToThreat.generated.h"

/**
 * Trace-based test: sprawdza, czy linia wzroku między punktem-kandydatem
 * a aktualną pozycją zagrożenia jest fizycznie zasłonięta.
 */
UCLASS()
class AICOMBATSANDBOX_API UEnvQueryTest_CoverAngleToThreat : public UEnvQueryTest
{
	GENERATED_BODY()

public:
	UEnvQueryTest_CoverAngleToThreat();

	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;
	virtual FText GetDescriptionTitle() const override;
	virtual FText GetDescriptionDetails() const override;
};