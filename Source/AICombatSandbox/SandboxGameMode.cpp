#include "SandboxGameMode.h"
#include "SandboxPlayerCharacter.h"

ASandboxGameMode::ASandboxGameMode()
{
    DefaultPawnClass = ASandboxPlayerCharacter::StaticClass();
}

/*void ASandboxGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    // TODO: NewPlayer->SetGenericTeamId(FGenericTeamId(0)); — inny team niż AI (1)
	NewPlayer->SetGenericTeamId(FGenericTeamId(0));
}*/