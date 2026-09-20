#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SandboxGameMode.generated.h"

UCLASS()
class AICOMBATSANDBOX_API ASandboxGameMode : public AGameModeBase
{
    GENERATED_BODY()
public:
    ASandboxGameMode();

    //virtual void PostLogin(APlayerController* NewPlayer) override;
};