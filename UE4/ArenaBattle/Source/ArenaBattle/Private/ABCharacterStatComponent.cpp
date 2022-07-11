// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacterStatComponent.h"
#include "ABGameInstance.h"


// Sets default values for this component's properties
UABCharacterStatComponent::UABCharacterStatComponent()
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
void UABCharacterStatComponent::SetNewLevel(const int32& NewLevel)
{
	const auto ABGameInstance = Cast<UABGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	ABCHECK(ABGameInstance);
	CurrentStatData = ABGameInstance->GetAbCharacterData(Level);
	if(CurrentStatData)
	{
		Level = NewLevel;
		SetHP(CurrentStatData->MaxHP);
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
void UABCharacterStatComponent::SetDamage(const float& NewDamage)
{
	ABCHECK(CurrentStatData);
	SetHP(FMath::Clamp<float>(CurrentHP - NewDamage, 0.0f, CurrentStatData->MaxHP));
}


/**
 * @brief 공격데미지량 반환 함수
 * @return 공격데미지량 반환
 */
float UABCharacterStatComponent::GetAttack() const
{
	ABCHECK(CurrentStatData, 0.0f);
	return CurrentStatData->Attack;
}


/**
 * @brief HP를 설정하고, 특정값 이하이면 죽었음을 알림.
 * @param NewHP 설정할 HP 
 */
void UABCharacterStatComponent::SetHP(const float& NewHP)
{
	CurrentHP = NewHP;
	OnHPChanged.Broadcast();
	if(CurrentHP < KINDA_SMALL_NUMBER)
	{
		CurrentHP = 0.0f;
		OnHPIsZero.Broadcast();
	}
}


/**
 * @brief 현재의 HP의 퍼센트를 반환 
 * @return 0~1로 정규화된 값
 */
float UABCharacterStatComponent::GetHPRatio() const
{
	ABCHECK(CurrentStatData, 0.0f);

	return (CurrentStatData->MaxHP < KINDA_SMALL_NUMBER) ? 0.0f : (CurrentHP / CurrentStatData->MaxHP);
}


// Called when the game starts
void UABCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UABCharacterStatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UABCharacterStatComponent::InitializeComponent()
{
	Super::InitializeComponent();

	SetNewLevel(Level);
}

