#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SandboxPlayerCharacter.generated.h"

class UAIPerceptionStimuliSourceComponent;

UCLASS()
class AICOMBATSANDBOX_API ASandboxPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASandboxPlayerCharacter();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "AI|Perception")
	TObjectPtr<UAIPerceptionStimuliSourceComponent> StimuliSource;
};