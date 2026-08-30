// MyRPGDemo


#include "Characters/WarriorEnemyCharacter.h"

#include "Engine/AssetManager.h"
#include "Component/Combat/EnemyCombatComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "WarriorDebugHelper.h"
#include "Widget/WarriorWidgetBase.h"

AWarriorEnemyCharacter::AWarriorEnemyCharacter()
{
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	GetCharacterMovement()->RotationRate = FRotator(0.f,180.f,0.f);
	GetCharacterMovement()->BrakingDecelerationWalking = 1000.f;

	EnemyCombatComponent = CreateDefaultSubobject<UEnemyCombatComponent>(TEXT("EnemyCombatComponent"));

	EnemyUIComponent = CreateDefaultSubobject<UEnemyUIComponent>(TEXT("EnemyUIComponent"));

	EnemyHealthWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("EnemyHealthWidgetComponent");
	EnemyHealthWidgetComponent->SetupAttachment(GetMesh());
}

UPawnCombatComponent* AWarriorEnemyCharacter::GetPawnCombatComponent() const
{
	return EnemyCombatComponent;
}

UPawnUIComponent* AWarriorEnemyCharacter::GetPawnUIComponent() const
{
	return EnemyUIComponent;
}

UEnemyUIComponent* AWarriorEnemyCharacter::GetEnemyUIComponent() const
{
	return EnemyUIComponent;
}

void AWarriorEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	// WidgetComponent 的 widget 可能晚于 BeginPlay 创建，延迟到下一帧再尝试绑定
	GetWorldTimerManager().SetTimerForNextTick(this, &AWarriorEnemyCharacter::TryBindEnemyHealthWidget);
}

void AWarriorEnemyCharacter::TryBindEnemyHealthWidget()
{
	if (UWarriorWidgetBase* HealthWidget = Cast<UWarriorWidgetBase>(EnemyHealthWidgetComponent->GetUserWidgetObject()))
	{
		UE_LOG(LogTemp, Warning, TEXT("[HUD] 敌人血条绑定成功: %s"), *GetName());
		HealthWidget->InitEnemyWidget(this);
	}
	else if (HealthWidgetBindRetryCount < 10)
	{
		// widget 还没创建，延迟重试（最多 10 次，间隔 0.2 秒）
		HealthWidgetBindRetryCount++;
		FTimerHandle RetryTimer;
		GetWorldTimerManager().SetTimer(
			RetryTimer,
			this,
			&AWarriorEnemyCharacter::TryBindEnemyHealthWidget,
			0.2f,
			false
		);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[HUD] 敌人血条绑定重试超时: %s"), *GetName());
	}
}

void AWarriorEnemyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitEnemyStartUpData();
}

void AWarriorEnemyCharacter::InitEnemyStartUpData()
{
	if (CharacterStartUpData.IsNull())
	{
		return;
	}

	UAssetManager::GetStreamableManager().RequestAsyncLoad(CharacterStartUpData.ToSoftObjectPath(),FStreamableDelegate::CreateLambda(
		[this]()
		{
			if (UDataAsset_StartUpDataBase* LoadedData = CharacterStartUpData.Get())
			{
				LoadedData->GiveToAbilitySystemComponent(WarriorAbilitySystemComponent);
			}
		}));
}
