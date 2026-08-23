#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SandboxPlayerCharacter.generated.h"

class UAIPerceptionStimuliSourceComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class AICOMBATSANDBOX_API ASandboxPlayerCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ASandboxPlayerCharacter();
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, Category = "AI|Perception")
    TObjectPtr<UAIPerceptionStimuliSourceComponent> StimuliSource;

    UPROPERTY(VisibleAnywhere, Category = "Camera")
    TObjectPtr<UCameraComponent> Camera;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputMappingContext> DefaultMappingContext;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputAction> MoveAction;

    void Move(const FInputActionValue& Value);

    UPROPERTY(EditDefaultsOnly, Category = "AI|Noise")
    float NoiseReportInterval = 0.5f;

    float TimeSinceLastNoise = 0.0f;

    UPROPERTY(EditDefaultsOnly, Category = "AI|Noise")
    float NoiseLoudness = 1.0f;

    UPROPERTY(EditDefaultsOnly, Category = "AI|Noise")
    float NoiseMaxRange = 1000.0f;
};