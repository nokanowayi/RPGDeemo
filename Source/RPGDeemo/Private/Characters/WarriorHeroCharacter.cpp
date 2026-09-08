// MyRPGDemo


#include "Characters/WarriorHeroCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "WarriorDebugHelper.h"
#include "WarriorGameplayTags.h"
#include "Components/CapsuleComponent.h"
#include "DataAssets/Input/DataAsset_InputConfig.h"
#include "Component/Input/WarrriorInputComponent.h"
#include "DataAssets/StartUpData/DataAsset_StartUpDataBase.h"
#include "Component/Combat/HeroCombatComponent.h"
#include "Component/UI/HeroUIComponent.h"
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

	HeroUIComponent = CreateDefaultSubobject<UHeroUIComponent>(TEXT("HeroUIComponent"));
	
}

UPawnCombatComponent* AWarriorHeroCharacter::GetPawnCombatComponent() const
{
	return HeroCombatComponent;
}

UPawnUIComponent* AWarriorHeroCharacter::GetPawnUIComponent() const
{
	return HeroUIComponent;
}

UHeroUIComponent* AWarriorHeroCharacter::GetHeroUIComponent() const
{
	return HeroUIComponent;
}

void AWarriorHeroCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// 只在服务器端给能力，客户端通过复制获取
	if (!CharacterStartUpData.IsNull() && HasAuthority())
	{
		if(UDataAsset_StartUpDataBase* LoadData = CharacterStartUpData.LoadSynchronous())
		{
			LoadData->GiveToAbilitySystemComponent(GetWarriorAbilitySystemComponent(), 1);
		}
	}
}

void AWarriorHeroCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 角色 Transform 由 CharacterMovement 自动复制，无需手动声明
	// 如需复制自定义属性（如装备状态），在这里 DOREPLIFETIME
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
	WarriorInputComponent->BindNativeInputAction(InputConfig, WarriorGameplayTags::InputTag_SwitchTarget, ETriggerEvent::Triggered,this, &AWarriorHeroCharacter::Input_SwitchTargetTriggered);
	WarriorInputComponent->BindNativeInputAction(InputConfig, WarriorGameplayTags::InputTag_SwitchTarget, ETriggerEvent::Completed,this, &AWarriorHeroCharacter::Input_SwitchTargetCompleted);
	WarriorInputComponent->BindNativeInputAction(InputConfig, WarriorGameplayTags::InputTag_Interact, ETriggerEvent::Started,this, &AWarriorHeroCharacter::Input_InteractStarted);
	
	WarriorInputComponent->BindAbilityInputAction(InputConfig, this, &AWarriorHeroCharacter::Input_AbilityInputPressed, &AWarriorHeroCharacter::Input_AbilityInputReleased);
}

void AWarriorHeroCharacter::BeginPlay()
{
	Super::BeginPlay();
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

void AWarriorHeroCharacter::Input_SwitchTargetTriggered(const FInputActionValue& InputActionValue)
{
	SwitchDirection = InputActionValue.Get<FVector2D>();
}

void AWarriorHeroCharacter::Input_SwitchTargetCompleted(const FInputActionValue& InputActionValue)
{
	FGameplayEventData Data;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this,  SwitchDirection.X > 0.1f?WarriorGameplayTags::Player_Event_SwitchTarget_Right:WarriorGameplayTags::Player_Event_SwitchTarget_Left, Data);
}

void AWarriorHeroCharacter::Input_InteractStarted(const FInputActionValue& InputActionValue)
{
	FGameplayEventData Data;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this,
		WarriorGameplayTags::Player_Event_ConsumeStone,
		 Data);

	//todo:改成交互而不是单独的石头
}

void AWarriorHeroCharacter::Input_AbilityInputPressed(FGameplayTag InInputTag)
{
	WarriorAbilitySystemComponent->OnAbilityInputPressed(InInputTag);
}

void AWarriorHeroCharacter::Input_AbilityInputReleased(FGameplayTag InInputTag)
{
	WarriorAbilitySystemComponent->OnAbilityInputReleased(InInputTag);
}


