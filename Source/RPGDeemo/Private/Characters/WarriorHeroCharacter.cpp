// MyRPGDemo


#include "Characters/WarriorHeroCharacter.h"

#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "WarriorDebugHelper.h"
#include "WarriorGameplayTags.h"
#include "Components/CapsuleComponent.h"
#include "DataAssets/Input/DataAsset_InputConfig.h"
#include "Component/Input/WarrriorInputComponent.h"
#include "DataAssets/StartUpData/DataAsset_StartUpDataBase.h"
#include "Component/Combat/HeroCombatComponent.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"

AWarriorHeroCharacter::AWarriorHeroCharacter()
{
	OverrideInputComponentClass = UWarrriorInputComponent::StaticClass();

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->SocketOffset = FVector(0.0f, 55.0f, 65.0f);
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = 400.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;

	HeroCombatComponent = CreateDefaultSubobject<UHeroCombatComponent>(TEXT("HeroCombatComponent"));
	
}

void AWarriorHeroCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (!CharacterStartUpData.IsNull())
	{
		if(UDataAsset_StartUpDataBase* LoadData = CharacterStartUpData.LoadSynchronous())
		{
			LoadData->GiveToAbilitySystemComponent(GetWarriorAbilitySystemComponent(), 1);
		}
	}
}

void AWarriorHeroCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	checkf(InputConfig, TEXT("InputConfig is null, please assign InputConfig in BP"));
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	ULocalPlayer* LocalPlayer = GetController<APlayerController>()->GetLocalPlayer();
	UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	Subsystem->AddMappingContext(InputConfig->DefautMappingContext, 0);

	UWarrriorInputComponent* WarriorInputComponent = CastChecked<UWarrriorInputComponent>(PlayerInputComponent);
	WarriorInputComponent->BindNativeInputAction(InputConfig, WarriorGameplayTags::InputTag_Move, ETriggerEvent::Triggered,this, &AWarriorHeroCharacter::Input_Move);
	WarriorInputComponent->BindNativeInputAction(InputConfig, WarriorGameplayTags::InputTag_Look, ETriggerEvent::Triggered,this, &AWarriorHeroCharacter::Input_Look);

	WarriorInputComponent->BindAbilityInputAction(InputConfig, this, &AWarriorHeroCharacter::Input_AbilityInputPressed, &AWarriorHeroCharacter::Input_AbilityInputReleased);
}

void AWarriorHeroCharacter::BeginPlay()
{
	Super::BeginPlay();
	Debug::Print(TEXT("Working"));
}

void AWarriorHeroCharacter::Input_Move(const FInputActionValue& InputActionValue)
{
	FVector2D MovementVector = InputActionValue.Get<FVector2D>();
	FRotator MovementRotator(0.0, Controller->GetControlRotation().Yaw, 0.0);

	if (MovementVector.Y != 0)
	{
		FVector ForwardVector = MovementRotator.RotateVector(FVector::ForwardVector);
		AddMovementInput(ForwardVector, MovementVector.Y);
	}

	if (MovementVector.X != 0)
	{
		FVector RightVector = MovementRotator.RotateVector(FVector::RightVector);
		AddMovementInput(RightVector, MovementVector.X);
	}
}

void AWarriorHeroCharacter::Input_Look(const FInputActionValue& InputActionValue)
{
	const FVector2D lookVector = InputActionValue.Get<FVector2D>();
	if (lookVector.X != 0)
	{
		AddControllerYawInput(lookVector.X);
	}
	if (lookVector.Y != 0)
	{
		AddControllerPitchInput(-lookVector.Y);
	}
}

void AWarriorHeroCharacter::Input_AbilityInputPressed(FGameplayTag InInputTag)
{
	WarriorAbilitySystemComponent->OnAbilityInputPressed(InInputTag);
}

void AWarriorHeroCharacter::Input_AbilityInputReleased(FGameplayTag InInputTag)
{
	WarriorAbilitySystemComponent->OnAbilityInputReleased(InInputTag);
}
