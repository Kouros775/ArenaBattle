// Fill out your copyright notice in the Description page of Project Settings.


#include "ABGameInstance.h"


UABGameInstance::UABGameInstance()
{
	FString CharacterDataPath = TEXT("");
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_ABCHARACTER(TEXT("DataTable'/Game/GameData/ABCharacterData.ABCharacterData'"));
	ABCHECK(DT_ABCHARACTER.Succeeded());
	ABCharacterTable = DT_ABCHARACTER.Object;
	ABCHECK(ABCharacterTable->GetRowMap().Num() > 0);
}

void UABGameInstance::Init()
{
	Super::Init();
	ABLOG_S(Warning);
	//ABLOG(Warning, TEXT("DropExp of Level 20 ABCharacter : %d"), GetAbCharacterData(20)->DropExp);
}


FABCharacterData* UABGameInstance::GetAbCharacterData(const int32& Level)
{
	return ABCharacterTable->FindRow<FABCharacterData>(*FString::FromInt(Level), TEXT(""));
}
