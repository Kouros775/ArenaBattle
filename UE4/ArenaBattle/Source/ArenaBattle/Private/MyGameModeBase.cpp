// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameModeBase.h"
#include "MyPlayerCharacter.h"


AMyGameModeBase::AMyGameModeBase()
{
	DefaultPawnClass = AMyPlayerCharacter::StaticClass();
}
