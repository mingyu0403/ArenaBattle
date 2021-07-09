#include "ABGameInstance.h"
#include "CustomDataTables.h"

UABGameInstance::UABGameInstance()
{
	FString CharacterDataPath = TEXT("DataTable'/Game/Book/GameData/ABCharacterData.ABCharacterData'");
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_ABCHARACTER(*CharacterDataPath);
	if (DT_ABCHARACTER.Succeeded())
	{
		ABCharacterTable = DT_ABCHARACTER.Object;
	}
}

void UABGameInstance::Init()
{
	Super::Init();
}

FABCharacterData* UABGameInstance::GetABCharacterData(int32 Level)
{
	return ABCharacterTable->FindRow<FABCharacterData>(*FString::FromInt(Level), TEXT(""));
}
