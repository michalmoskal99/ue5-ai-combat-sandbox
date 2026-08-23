#include "SandboxPlayerCharacter.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Camera/CameraComponent.h"           // NOWE
#include "EnhancedInputComponent.h"            // NOWE
#include "EnhancedInputSubsystems.h"            // NOWE
#include "InputAction.h"                        // NOWE
#include "InputMappingContext.h"                // NOWE
#include "InputActionValue.h"                   // NOWE
#include "Components/CapsuleComponent.h"

ASandboxPlayerCharacter::ASandboxPlayerCharacter()
{
    StimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimuliSource"));

    // NOWE — kamera na wysokości oczu, przypięta do kapsuły (brak mesha, ale widok już działa)
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(GetCapsuleComponent());
    Camera->SetRelativeLocation(FVector(0.f, 0.f, 60.f));

    // NOWE — te same ConstructorHelpers co przy BT_Sandbox/DA_SandboxAIParams
    static ConstructorHelpers::FObjectFinder<UInputMappingContext> IMCObj(TEXT("/Game/Input/IMC_Player.IMC_Player"));
    if (IMCObj.Succeeded())
    {
        DefaultMappingContext = IMCObj.Object;
    }

    static ConstructorHelpers::FObjectFinder<UInputAction> MoveActionObj(TEXT("/Game/Input/IA_Move.IA_Move"));
    if (MoveActionObj.Succeeded())
    {
        MoveAction = MoveActionObj.Object;
    }
}

void ASandboxPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    StimuliSource->RegisterForSense(UAISense_Hearing::StaticClass());
    StimuliSource->RegisterForSense(UAISense_Sight::StaticClass());
    StimuliSource->RegisterWithPerceptionSystem();

    // NOWE — podpięcie Input Mapping Context przy starcie gry
    if (APlayerController* PC = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            if (DefaultMappingContext)
            {
                Subsystem->AddMappingContext(DefaultMappingContext, 0);
            }
        }
    }
}

// NOWE — cała funkcja
void ASandboxPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASandboxPlayerCharacter::Move);
    }
}

// NOWE — cała funkcja
void ASandboxPlayerCharacter::Move(const FInputActionValue& Value)
{
    const FVector2D MovementVector = Value.Get<FVector2D>();

    if (Controller)
    {
        AddMovementInput(GetActorForwardVector(), MovementVector.Y);
        AddMovementInput(GetActorRightVector(), MovementVector.X);
    }
}

void ASandboxPlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (GetVelocity().Size() > 10.f)
    {
        TimeSinceLastNoise += DeltaTime;

        if (TimeSinceLastNoise >= NoiseReportInterval)
        {
            UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), NoiseLoudness, this, NoiseMaxRange);
            UE_LOG(LogTemp, Warning, TEXT("Noise reported at: %s"), *GetActorLocation().ToString());

            TimeSinceLastNoise = 0.0f;
        }
    }
    else
    {
        TimeSinceLastNoise = 0.0f;
    }
}