#include "SandboxPlayerCharacter.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"

ASandboxPlayerCharacter::ASandboxPlayerCharacter()
{
	StimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimuliSource"));
}

void ASandboxPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// TODO: StimuliSource->RegisterForSense(UAISense_Sight::StaticClass());
	StimuliSource->RegisterForSense(UAISense_Hearing::StaticClass());
	// TODO: StimuliSource->RegisterForSense(UAISense_Hearing::StaticClass());
	StimuliSource->RegisterForSense(UAISense_Sight::StaticClass());
	// TODO: StimuliSource->RegisterWithPerceptionSystem();
	StimuliSource->RegisterWithPerceptionSystem();
}