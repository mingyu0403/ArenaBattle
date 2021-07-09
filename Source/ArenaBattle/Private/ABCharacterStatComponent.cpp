#include "ABCharacterStatComponent.h"

#include "ABGameInstance.h"
#include "CustomDataTables.h"

UABCharacterStatComponent::UABCharacterStatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;

	Level = 1;
}

void UABCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UABCharacterStatComponent::InitializeComponent()
{
	Super::InitializeComponent();
	SetNewLevel(Level);
}

void UABCharacterStatComponent::SetNewLevel(int32 NewLevel)
{
	UABGameInstance* ABGameInstance = Cast<UABGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (nullptr == ABGameInstance) return;

	CurrentStatData = ABGameInstance->GetABCharacterData(NewLevel);
	if (CurrentStatData)
	{
		Level = NewLevel;
		SetHP(CurrentStatData->MaxHP);
		CurrentHP = CurrentStatData->MaxHP;
	}
	else
	{
		UE_LOG(LogClass, Warning, TEXT("Level %d data doesn't exist."), NewLevel);
	}
}

void UABCharacterStatComponent::SetDamage(float NewDamage)
{
	if (nullptr == CurrentStatData) return;

	SetHP(FMath::Clamp<float>(CurrentHP - NewDamage, 0.0f, CurrentStatData->MaxHP));
}

void UABCharacterStatComponent::SetHP(float NewHP)
{
	CurrentHP = NewHP;
	OnHPChanged.Broadcast();
	if (CurrentHP < KINDA_SMALL_NUMBER)
	{
		CurrentHP = 0.0f;
		OnHPIsZero.Broadcast();
	}
}

float UABCharacterStatComponent::GetAttack() const
{
	if (nullptr == CurrentStatData) return 0.0f;

	return CurrentStatData->Attack;
}

float UABCharacterStatComponent::GetHPRatio() const
{
	if (nullptr == CurrentStatData) return 0.0f;

	return (CurrentStatData->MaxHP < KINDA_SMALL_NUMBER) ? 0.0f : (CurrentHP / CurrentStatData->MaxHP);
}

int32 UABCharacterStatComponent::GetDropExp() const
{
	return CurrentStatData->DropExp;
}

