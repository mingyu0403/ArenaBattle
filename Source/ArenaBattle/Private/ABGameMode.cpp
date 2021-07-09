// Fill out your copyright notice in the Description page of Project Settings.


#include "ABGameMode.h"
#include "ABCharacter.h"
#include "ABPlayerController.h"
#include "ABPlayerState.h"

AABGameMode::AABGameMode()
{
	DefaultPawnClass = AABCharacter::StaticClass();
	PlayerControllerClass = AABPlayerController::StaticClass();
	PlayerStateClass = AABPlayerState::StaticClass();
}

void AABGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	AABPlayerState* ABPlayerState = Cast<AABPlayerState>(NewPlayer->PlayerState);
	if (nullptr == ABPlayerState) return;

	ABPlayerState->InitPlayerData();
}
