// SandboxSquadSubsystem.h
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "SandboxSquadSubsystem.generated.h"

class ASandboxAIController;

UCLASS()
class AICOMBATSANDBOX_API USandboxSquadSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	void RegisterMember(ASandboxAIController* Controller);
	void UnregisterMember(ASandboxAIController* Controller);
	void BroadcastLastKnownLocation(ASandboxAIController* Source, const FVector& Location);
	void CallForHelp(const FVector& Location, float Radius);

private:
	// TODO: TWeakObjectPtr, nie surowy wskaźnik — pomyśl, czemu (NPC może zginąć,
	// subsystem nie ma się o tym dowiadywać przez destruktor)
	UPROPERTY()
	TArray<TWeakObjectPtr<ASandboxAIController>> SquadMembers;
};