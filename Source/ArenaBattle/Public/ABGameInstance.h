// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "Engine/GameInstance.h"
#include "Engine/StreamableManager.h"
#include "ABGameInstance.generated.h"

struct FABCharacterData;
class UDataTable;

UCLASS()
class ARENABATTLE_API UABGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UABGameInstance();

	virtual void Init() override;
	FABCharacterData* GetABCharacterData(int32 Level);

	FStreamableManager StreamableManager;

private:
	UPROPERTY()
	UDataTable* ABCharacterTable;
};
