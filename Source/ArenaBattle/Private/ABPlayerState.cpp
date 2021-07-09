#include "ABPlayerState.h"
#include "ABGameInstance.h"
#include "CustomDataTables.h"

AABPlayerState::AABPlayerState()
{
	CharacterLevel = 1;
	GameScore = 0;
	Exp = 0;
}

int32 AABPlayerState::GetGameScore() const
{
	return GameScore;
}

int32 AABPlayerState::GetCharacterLevel() const
{
	return CharacterLevel;
}

float AABPlayerState::GetExpRatio() const
{
	if (CurrentStatData->NextExp <= KINDA_SMALL_NUMBER)
	{
		return 0.0f;
	}

	float Result = (float)Exp / (float)CurrentStatData->NextExp;
	UE_LOG(LogClass, Warning, TEXT("Ratio : %f, Current : %d, Next : %d"), Result, Exp, CurrentStatData->NextExp);
	return Result;
}

bool AABPlayerState::AddExp(int32 IncomeExp)
{
	if (CurrentStatData->NextExp == -1)
	{
		return false;
	}

	bool DidLevelUp = false;
	Exp = Exp + IncomeExp;
	if (Exp >= CurrentStatData->NextExp)
	{
		Exp -= CurrentStatData->NextExp;
		SetCharacterLevel(CharacterLevel + 1);
		DidLevelUp = true;
	}

	OnPlayerStateChanged.Broadcast();
	return DidLevelUp;
}

void AABPlayerState::InitPlayerData()
{
	SetPlayerName(TEXT("Destiny"));
	SetCharacterLevel(5);
	GameScore = 0;
	Exp = 0;
}

void AABPlayerState::SetCharacterLevel(int32 NewCharacterLevel)
{
	auto ABGameInstance = Cast<UABGameInstance>(GetGameInstance());
	if(nullptr == ABGameInstance) return;

	CurrentStatData = ABGameInstance->GetABCharacterData(NewCharacterLevel);
	if (nullptr == CurrentStatData) return;

	CharacterLevel = NewCharacterLevel;
}
