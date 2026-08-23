#include "SandboxGameMode.h"
#include "SandboxPlayerCharacter.h"

ASandboxGameMode::ASandboxGameMode()
{
    DefaultPawnClass = ASandboxPlayerCharacter::StaticClass();
}