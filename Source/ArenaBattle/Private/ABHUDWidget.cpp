#include "ABHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "ABCharacterStatComponent.h"
#include "ABPlayerState.h"

void UABHUDWidget::BindCharacterStat(UABCharacterStatComponent* CharacterStat)
{
	if (nullptr == CharacterStat) return;
	CurrentCharacterStat = CharacterStat;
	CharacterStat->OnHPChanged.AddUObject(this, &UABHUDWidget::UpdateCharacterStat);
}

void UABHUDWidget::BindPlayerState(AABPlayerState* PlayerState)
{
	if (nullptr == PlayerState) return;
	CurrentPlayerState = PlayerState;
	PlayerState->OnPlayerStateChanged.AddUObject(this, &UABHUDWidget::UpdatePlayerState);
}

void UABHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
	HPBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("pbHP")));
	if (nullptr == HPBar) return;

	ExpBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("pbExp")));
	if (nullptr == ExpBar) return;

	PlayerName = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtPlayerName")));
	if (nullptr == PlayerName) return;

	PlayerLevel = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtLevel")));
	if (nullptr == PlayerLevel) return;

	CurrentScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtCurrentScore")));
	if (nullptr == CurrentScore) return;

	HighScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtHighScore")));
	if (nullptr == HighScore) return;
}

void UABHUDWidget::UpdateCharacterStat()
{
	if (false == CurrentCharacterStat.IsValid()) return;
	
	HPBar->SetPercent(CurrentCharacterStat->GetHPRatio());
}

void UABHUDWidget::UpdatePlayerState()
{
	if (false == CurrentPlayerState.IsValid()) return;

	ExpBar->SetPercent(CurrentPlayerState->GetExpRatio());
	PlayerName->SetText(FText::FromString(CurrentPlayerState->GetPlayerName()));
	PlayerLevel->SetText(FText::FromString(FString::FromInt(CurrentPlayerState->GetCharacterLevel())));
	CurrentScore->SetText(FText::FromString(FString::FromInt(CurrentPlayerState->GetGameScore())));
}
