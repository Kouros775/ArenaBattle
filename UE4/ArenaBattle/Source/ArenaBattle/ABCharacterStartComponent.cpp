// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacterStartComponent.h"
#include "ABGameInstance.h"


// Sets default values for this component's properties
UABCharacterStartComponent::UABCharacterStartComponent()
	: CurrentStatData(nullptr)
	, Level(1)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// 이 변수가 true여야 InitializeComponent이게 호출됨.
	bWantsInitializeComponent = true;
	// ...
}


/**
 * @brief Level로 설정하고, 그것에 맞는 CharacterData를 가져온다.
 * @param NewLevel 설정할 Level 
 */
void UABCharacterStartComponent::SetNewLevel(const int32& NewLevel)
{
	const auto ABGameInstance = Cast<UABGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	ABCHECK(ABGameInstance);
	CurrentStatData = ABGameInstance->GetAbCharacterData(Level);
	if(CurrentStatData)
	{
		Level = NewLevel;
		CurrentHP = CurrentStatData->MaxHP;
	}
	else
	{
		ABLOG(Error, TEXT("Level(%d) data doesn't exist"), NewLevel);
	}
}


/**
 * @brief 데미지를 입고, Hp가 0이면 죽었다고 알린다.
 * @param NewDamage 타격받은 데미지 량 
 */
void UABCharacterStartComponent::SetDamage(const float& NewDamage)
{
	ABCHECK(CurrentStatData);
	CurrentHP = FMath::Clamp<float>(CurrentHP - NewDamage, 0.0f, CurrentStatData->MaxHP);
	if(CurrentHP <= FLT_EPSILON)
	{
		OnHPIsZero.Broadcast();
	}
	else
	{
		// nothing. 살아있음.
	}
}


/**
 * @brief 공격데미지량 반환 함수
 * @return 공격데미지량 반환
 */
float UABCharacterStartComponent::GetAttack() const
{
	ABCHECK(CurrentStatData, 0.0f);
	return CurrentStatData->Attack;
}


// Called when the game starts
void UABCharacterStartComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UABCharacterStartComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UABCharacterStartComponent::InitializeComponent()
{
	Super::InitializeComponent();

	SetNewLevel(Level);
}

