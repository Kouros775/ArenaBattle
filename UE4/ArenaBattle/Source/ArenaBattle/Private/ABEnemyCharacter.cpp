// Fill out your copyright notice in the Description page of Project Settings.


#include "ABEnemyCharacter.h"

#include <memory>

#include "ABWeapon.h"
#include "ABGameInstance.h"
#include "ABAnimInstance.h"
#include "ABAIController.h"
#include "ABCharacterWidget.h"
#include "ABCharacterSetting.h"
#include "ABCharacterStatComponent.h"
#include "Components/WidgetComponent.h"
#include "ABPlayerController.h"
#include "ABPlayerState.h"
#include "ABHUDWidget.h"
#include "ABGameMode.h"

//#include "DrawDebugHelpers.h"


/**
 * @brief ������
 */
AABEnemyCharacter::AABEnemyCharacter()
	: DirectionToMove(FVector::ZeroVector)
	, IsAttacking(false)
	, MaxCombo(4)
	, AttackRange(80.0f)
	, AttackRadius(50.0f)
	, DeadTimer(5.0f)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// >> Init Camera
	SetControlMode();
	// << Init Camera

	// >> Inti UI
	_initUI();
	// << Inti UI

	// >> Init Mesh
	_initMesh();
	// << Init Mesh

	// >> Init Animation
	_initAnimation();
	AttackEndComboState();
	// << Init Animation

	// >> Init Character Stat
	_initCharacterStat();
	// << Init Character Stat

	// >> Init Physics
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("ABCharacter"));
	// << Init Physics

	// >> Init Controller
	_initController();
	// << Init Controller





	// >> ArenaBattle Setting
	//const auto DefaultSetting = GetDefault<UABCharacterSetting>();
	//if(DefaultSetting->CharacterAssets.Num() > 0)
	//{
	//	for(auto CharacterAsset : DefaultSetting->CharacterAssets)
	//	{
	//		ABLOG(Warning, TEXT("Character Asset : %s"), *CharacterAsset.ToString());
	//	}
	//}

	CharacterAssetToLoad = FSoftObjectPath(nullptr);
	// << ArenaBattle Setting


	// >> Preint state
	SetActorHiddenInGame(true);
	HPBarWidget->SetHiddenInGame(true);
	//bCanBeDamaged = false;
	SetCanBeDamaged(false);
	// << Preint state
}


// Called when the game starts or when spawned
void AABEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();


	ABAIController = Cast<AABAIController>(GetController());
	ABCHECK(ABAIController);


	const auto DefaultSetting = GetDefault<UABCharacterSetting>();


	int nAssetIndex = FMath::RandRange(0, DefaultSetting->CharacterAssets.Num() - 1);

	CharacterAssetToLoad = DefaultSetting->CharacterAssets[nAssetIndex];
	const auto ABGameInstance = Cast<UABGameInstance>(GetGameInstance());
	ABCHECK(ABGameInstance);
	AssetStreamingHandle = ABGameInstance->StreamableManager.RequestAsyncLoad(CharacterAssetToLoad
		, FStreamableDelegate::CreateUObject(this, &AABEnemyCharacter::OnAssetLoadCompleted));
	SetCharacterState(ECharacterState::Loading);
}


/**
 * @brief ī�޶� ���� ����
 * @param ControlMode ī�޶� ����
 */
void AABEnemyCharacter::SetControlMode()
{
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 480.0f, 0.0f);
}


// Called every frame
void AABEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


// Called to bind functionality to input
void AABEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


void AABEnemyCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	ABAnim = Cast<UABAnimInstance>(GetMesh()->GetAnimInstance());
	ABCHECK(nullptr != ABAnim);
	ABAnim->OnMontageEnded.AddDynamic(this, &AABEnemyCharacter::OnAttackMontageEnded); // �ִϸ��̼� ��Ÿ�� ����� ������ ȣ��Ǵ� �ݹ��Լ�.

	ABAnim->OnNextAttackCheck.AddLambda([this]()->void
		{
			ABLOG(Warning, TEXT("OnNextAttackCheck"));
			CanNextCombo = false;

			if (IsComboInputOn)
			{
				AttackStartComboState();
				ABAnim->JumpToAttackMontageSection(CurrentCombo);
			}
		});

	// Attack�� Collision Ȯ��.
	ABAnim->OnAttackHitCheck.AddUObject(this, &AABEnemyCharacter::AttackCheck);

	// HP�� 0�϶� ȣ���� �Լ�.
	CharacterStat->OnHPIsZero.AddLambda([this]()->void
		{
			ABLOG(Warning, TEXT("OnHPIsZero"));
			ABAnim->SetDeadAnim();
			SetActorEnableCollision(false);
		});
}


void AABEnemyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	
	SetControlMode();
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
}


/**
 * @brief �������� �ִ� �Լ�
 * @param DamageAmount ��������
 * @param DamageEvent �������� ����
 * @param EventInstigator ������(������ ����� ���� ��Ʈ�ѷ�)
 * @param DamageCauser ������ ������ ���� ����� ����(���� �� ĳ���Ͱ� ������ �ȴ�.)
 * @return ���� ���� ������.
 */
float AABEnemyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	ABLOG(Warning, TEXT("Actor : %s took Damage : %f"), *GetName(), FinalDamage);

	CharacterStat->SetDamage(FinalDamage);

	if (CurrentState == ECharacterState::Dead)
	{
		if (EventInstigator->IsPlayerController())
		{
			auto PlayerController = Cast<AABPlayerController>(EventInstigator);
			//ABCHECK(nullptr != PlayerController);
			//PlayerController->NPCKill(this);
		}
	}


	return FinalDamage;
}


/**
 * @brief ���⸦ ������ �� �ִ����� ���� ���� ��ȯ
 * @return ������������ ���� true/false
 */
bool AABEnemyCharacter::CanSetWeapon() const
{
	//return (nullptr == CurrentWeapon);
	return true;
}


/**
 * @brief ĳ���Ϳ� ���⸦ �����Ѵ�.
 * @param NewWeapon �����ϴ� ����
 */
void AABEnemyCharacter::SetWeapon(AABWeapon* NewWeapon)
{
	ABCHECK(nullptr != NewWeapon);
	if (nullptr != CurrentWeapon)
	{
		CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		CurrentWeapon->Destroy();
		CurrentWeapon = nullptr;
	}



	const FName WeaponSocket(TEXT("hand_rSocket"));
	if (NewWeapon)
	{
		NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
		NewWeapon->SetOwner(this);
		CurrentWeapon = NewWeapon;
	}
}


/**
 * @brief Attack
 */
void AABEnemyCharacter::Attack()
{
	if (IsAttacking)
	{
		ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 1, MaxCombo));

		if (CanNextCombo)
		{
			IsComboInputOn = true;
		}
	}
	else
	{
		ABCHECK(CurrentCombo == 0);
		AttackStartComboState();
		ABAnim->PlayAttackMontage();
		ABAnim->JumpToAttackMontageSection(CurrentCombo);
		IsAttacking = true;
	}
}

void AABEnemyCharacter::SetCharacterState(const ECharacterState& NewState)
{
	ABCHECK(CurrentState != NewState);
	CurrentState = NewState;

	switch (CurrentState)
	{
	case ECharacterState::Loading:
	{
		auto ABGameMode = Cast<AABGameMode>(GetWorld()->GetAuthGameMode());
		ABCHECK(nullptr != ABGameMode);
		int32 TargetLevel = FMath::CeilToInt((float)ABGameMode->GetScore() * 0.8f);
		int32 FinalLevel = FMath::Clamp<int32>(TargetLevel, 1, 20);
		ABLOG(Warning, TEXT("New NPC Level : %d"), FinalLevel);
		CharacterStat->SetNewLevel(FinalLevel);


		SetActorHiddenInGame(true);
		HPBarWidget->SetHiddenInGame(true);
		SetCanBeDamaged(false);
		break;
	}
	case ECharacterState::Ready:
	{
		SetActorHiddenInGame(false);
		HPBarWidget->SetHiddenInGame(false);
		SetCanBeDamaged(true);

		CharacterStat->OnHPIsZero.AddLambda([this]()->void
			{
				SetCharacterState(ECharacterState::Dead);
			});

		const auto CharacterWidget = Cast<UABCharacterWidget>(HPBarWidget->GetUserWidgetObject());
		ABCHECK(nullptr != CharacterWidget);
		CharacterWidget->BindCharacterStat(CharacterStat);

		
		SetControlMode();
		GetCharacterMovement()->MaxWalkSpeed = 400.0f;
		ABAIController->RunAI();

		break;
	}
	case ECharacterState::Dead:
	{
		SetActorEnableCollision(false);
		GetMesh()->SetHiddenInGame(false);
		HPBarWidget->SetHiddenInGame(true);
		ABAnim->SetDeadAnim();
		SetCanBeDamaged(false);

		ABAIController->StopAI();


		GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda([this]()->void
			{
				Destroy();
			}), DeadTimer, false);
		break;
	}

	default:
	{
		break;
	}
	}
}


ECharacterState AABEnemyCharacter::GetCharacterState() const
{
	return CurrentState;
}


void AABEnemyCharacter::AttackCheck()
{
	float FinalAttackRange = GetFinalAttackRange();

	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);

	bool bResult = GetWorld()->SweepSingleByChannel(
		HitResult
		, GetActorLocation()
		, GetActorLocation() + GetActorForwardVector() * FinalAttackRange
		, FQuat::Identity
		, ECollisionChannel::ECC_GameTraceChannel2
		, FCollisionShape::MakeSphere(50.0f)
		, Params
	);

//#if ENABLE_DRAW_DEBUG
//	FVector TraceVec = GetActorForwardVector() * FinalAttackRange;
//	FVector Center = GetActorLocation() + TraceVec * 0.5f;
//	float HalfHeight = FinalAttackRange * 0.5f + AttackRadius;
//	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
//	FColor DrawColor = bResult ? FColor::Red : FColor::Green;
//	float DebugLifeTime = 5.0f;
//
//	DrawDebugCapsule(GetWorld()
//		, Center
//		, HalfHeight
//		, AttackRadius
//		, CapsuleRot
//		, DrawColor
//		, false
//		, DebugLifeTime);
//#endif



	if (bResult)
	{
		if (HitResult.Actor.IsValid())
		{
			ABLOG(Warning, TEXT("Hit Actor Name : %s"), *HitResult.Actor->GetName());

			FDamageEvent DamageEvent;
			HitResult.Actor->TakeDamage(GetFinalAttackDamage(), DamageEvent, GetController(), this);
		}
	}
}


void AABEnemyCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	ABCHECK(IsAttacking);
	ABCHECK(CurrentCombo > 0);
	IsAttacking = false;
	AttackEndComboState();
	OnAttackEnd.Broadcast();
}


void AABEnemyCharacter::AttackStartComboState()
{
	CanNextCombo = true;
	IsComboInputOn = false;
	ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 0, MaxCombo - 1));
	CurrentCombo = FMath::Clamp<int32>(CurrentCombo + 1, 1, MaxCombo);
}


void AABEnemyCharacter::AttackEndComboState()
{
	IsComboInputOn = false;
	CanNextCombo = false;
	CurrentCombo = 0;
}


void AABEnemyCharacter::OnAssetLoadCompleted()
{
	USkeletalMesh* AssetLoaded = Cast<USkeletalMesh>(AssetStreamingHandle->GetLoadedAsset());
	AssetStreamingHandle.Reset();
	ABCHECK(AssetLoaded != nullptr);
	GetMesh()->SetSkeletalMesh(AssetLoaded);

	SetCharacterState(ECharacterState::Ready);
}


/**
 * @brief UI �ʱ�ȭ
 */
void AABEnemyCharacter::_initUI()
{
	HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBARWIDGET"));
	HPBarWidget->SetupAttachment(GetMesh());

	HPBarWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 180.0f));
	HPBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
	static ConstructorHelpers::FClassFinder<UUserWidget> UI_HUD(TEXT("WidgetBlueprint'/Game/UI/UI_HPBar.UI_HPBar_C'"));
	if (UI_HUD.Succeeded())
	{
		HPBarWidget->SetWidgetClass(UI_HUD.Class);
		HPBarWidget->SetDrawSize(FVector2D(150.0f, 50.0f));
	}
}


/**
 * @brief Mesh �ʱ�ȭ
 */
void AABEnemyCharacter::_initMesh()
{
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_Cardboard(TEXT("SkeletalMesh'/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Barbarous.SK_CharM_Barbarous'"));
	if (SK_Cardboard.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_Cardboard.Object);
	}
}


/**
 * @brief Animation �ʱ�ȭ
 */
void AABEnemyCharacter::_initAnimation()
{
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FClassFinder<UAnimInstance> WARRIOR_ANIM(TEXT("AnimBlueprint'/Game/Book/Animations/WarriorAnimBlueprint.WarriorAnimBlueprint_C'"));
	if (WARRIOR_ANIM.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(WARRIOR_ANIM.Class);
	}
}


/**
 * @brief Character ���� �ʱ�ȭ
 */
void AABEnemyCharacter::_initCharacterStat()
{
	CharacterStat = CreateDefaultSubobject<UABCharacterStatComponent>(TEXT("CHARACTERSTAT"));
	GetCharacterMovement()->JumpZVelocity = 400.0f;
}


/**
 * @brief ��������(�浹 ����) �ʱ�ȭ
 */
void AABEnemyCharacter::_initPhysics()
{
}


/**
 * @brief ��Ʈ�ѷ� �ʱ�ȭ
 */
void AABEnemyCharacter::_initController()
{
	// �÷��̾ �����ϴ� ĳ���͸� ������ ��� ĳ���Ϳ� ai set
	AIControllerClass = AABAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}


int32 AABEnemyCharacter::GetExp() const
{
	return CharacterStat->GetDropExp();
}

float AABEnemyCharacter::GetFinalAttackRange() const
{
	float attackRange;

	if (nullptr != CurrentWeapon)
		attackRange = CurrentWeapon->GetAttackRange();
	else
	{
		attackRange = this->AttackRange;
	}

	return attackRange;
}

float AABEnemyCharacter::GetFinalAttackDamage() const
{
	float rtn = 0.0f;

	if (nullptr != CurrentWeapon)
	{
		rtn = (CharacterStat->GetAttack() + CurrentWeapon->GetAttackDamage()) * CurrentWeapon->GetAttackModifier();
	}
	else
	{
		rtn = CharacterStat->GetAttack() * 1.0f;
	}

	return rtn;
}
