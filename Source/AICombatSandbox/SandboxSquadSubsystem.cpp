// Fill out your copyright notice in the Description page of Project Settings.


#include "SandboxSquadSubsystem.h"
#include "SandboxAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

// .cpp
void USandboxSquadSubsystem::RegisterMember(ASandboxAIController* Controller)
{
	// TODO: AddUnique + walidacja Controller
	if (Controller)
	{
		SquadMembers.AddUnique(Controller);
	}
	
}

void USandboxSquadSubsystem::UnregisterMember(ASandboxAIController* Controller)
{
	// TODO: Remove
	SquadMembers.RemoveSingle(Controller);
}

void USandboxSquadSubsystem::BroadcastLastKnownLocation(ASandboxAIController* Source, const FVector& Location)
{
	for (const TWeakObjectPtr<ASandboxAIController>& Member : SquadMembers)
	{
		if (Member.IsValid() && Member.Get() != Source)
		{
			Member->GetBlackboardComponent()->SetValueAsVector("LastKnownLocation", Location);
		}
	}
}

void USandboxSquadSubsystem::CallForHelp(const FVector& Location, float Radius)
{
	for (const TWeakObjectPtr<ASandboxAIController>& Member : SquadMembers)
	{
		if (Member.IsValid())
		{
			APawn* Pawn = Member->GetPawn();
			if (Pawn)
			{
				float Distance = FVector::Dist(Pawn->GetActorLocation(), Location);
				EAIState CurrentState = static_cast<EAIState>(Member->GetBlackboardComponent()->GetValueAsEnum(TEXT("AIState")));
				if (Distance <= Radius && CurrentState != EAIState::Alert && CurrentState != EAIState::Search && CurrentState != EAIState::Combat)
				{
					Member->GetBlackboardComponent()->SetValueAsEnum(TEXT("AIState"), static_cast<uint8>(EAIState::Alert));
					Member->GetBlackboardComponent()->SetValueAsVector(TEXT("LastKnownLocation"), Location);
				}
			}
		}
	}
}